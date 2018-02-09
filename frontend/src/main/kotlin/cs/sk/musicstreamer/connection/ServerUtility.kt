package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import kotlinx.coroutines.experimental.launch
import kotlinx.coroutines.experimental.nio.aConnect
import kotlinx.coroutines.experimental.nio.aRead
import kotlinx.coroutines.experimental.nio.aWrite
import mu.KLogging
import java.net.InetAddress
import java.net.InetSocketAddress
import java.nio.ByteBuffer
import java.nio.channels.AsynchronousSocketChannel
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.CopyOnWriteArrayList
import java.util.concurrent.atomic.AtomicBoolean


class Sock(
        private val host: String,
        private val port: Int,
        private val isConnected: AtomicBoolean = AtomicBoolean(false),
        private val bufferSize: Int = 256) {
    private var socket: AsynchronousSocketChannel = AsynchronousSocketChannel.open()

    companion object : KLogging()

    protected val socketDescription = "$host:$port"

    suspend fun read(): String {
        return withSocket { it.readSocket() }
    }

    suspend fun write(request: Request): Boolean {
        return withSocket { it.write(request) } >= 0
    }

    private suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = ServerConnector.objectMapper.writeValueAsString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return aWrite(buf = bytes).apply {
            if (this < 0) throw Error("Could not write to $socketDescription")
        }
    }

    fun ByteBuffer.asString() = String(array())


    private suspend fun AsynchronousSocketChannel.readSocket(): String {
        val buf = ByteBuffer.allocate(bufferSize)
        val read = aRead(buf)
        return if (read >= 0) buf.asString() else throw Error("Could not read from $socketDescription")
    }

    private suspend fun <T> withSocket(f: suspend (AsynchronousSocketChannel) -> T): T {
        connect()
        return f(socket)
    }

    @Synchronized
    suspend fun connect() {
        if (!isConnected.getAndSet(true)) {
            try {
                socket = AsynchronousSocketChannel.open()
                logger.info { "Initializing connection..." }
                socket.aConnect(InetSocketAddress(InetAddress.getByName(host), port))
                logger.info { "Successfully connected with $socketDescription" }
            } catch (e: Error) {
                isConnected.set(false)
                logger.error("could not connect to $socketDescription", e)
                throw Error("Could not connect")
            }
        }
    }
}

interface MessageListener {
    fun onNewMessage(status: Int, id: String?, message: JsonNode)
    fun onClose()
    fun manage(status: Int, id: String?, message: JsonNode, listener: ResponseListener) =
            with(listener) {
                when {
                    status in 400..499 -> onError(ErrorResponse(status, message.get("body").get("error").asText()))
                    status >= 500 -> onError(ErrorResponse(status, "Internal server error"))
                    else -> onResponse(status, id, message)
                }
            }
}

class UndefinedMessageListener : MessageListener {

    private val responseListeners = CopyOnWriteArrayList<ResponseListener>()
    override fun onClose() {}

    override fun onNewMessage(status: Int, id: String?, message: JsonNode) {
        if (id == null) {
            responseListeners.forEach {
                manage(status, id, message, it)
            }
        }
    }

    fun registerListener(listener: ResponseListener) {
        responseListeners += listener
    }

}

class IdentifiedMessageListener : MessageListener {
    companion object : KLogging()

    private val responsesListeners = ConcurrentHashMap<String, ResponseListener>()
    override fun onClose() {
        responsesListeners.forEach {
            it.value.onError(ErrorResponse(500, "Server closed"))
        }
    }

    override fun onNewMessage(status: Int, id: String?, message: JsonNode) {
        id?.let {
            responsesListeners[id]?.let {
                manage(status, id, message, it)
                responsesListeners.remove(id)
            } ?: logger.error { "listener for request $id was not found!" }
        }
    }

    fun waitForResponse(request: Request, listener: ResponseListener) =
            responsesListeners.put(request.id, listener)
}

class ResponseListener(
        val onResponse: (status: Int, id: String?, message: JsonNode) -> Unit,
        val onError: (ErrorResponse) -> Unit = {}
)

class SocketReader(
        private val socket: Sock,
        private val listeners: List<MessageListener>,
        private val bufferSize: Int = 256
) {

    companion object : KLogging() {
        val objectMapper = ObjectMapper()
    }

    private val isRunning = AtomicBoolean(false)
    fun initialize() {
        if (!isRunning.getAndSet(true)) {
            launch {
                while (isRunning.get()) {
                    consume(socket)
                }
            }
        }
    }

    fun shutDown() = isRunning.set(false)

    private suspend fun consume(socket: Sock) {
        var message = socket.read()
        var startFlag = 0
        var startIndex = 0

        var i = 0
        while (i < message.length) {
            when (message[i]) {
                '{' -> {
                    if (startFlag == 0) {
                        startIndex = i
                    }
                    startFlag++
                }
                '}' -> {
                    startFlag--
                    if (startFlag < 0 || (startFlag == 0 && startIndex > i)) {
                        logger.error { "malformed input: $message" }
                        return
                    } else if (startFlag == 0) {
                        val subMessage = message.substring(startIndex, i)
                        launch { manageMessage(subMessage) }
                        message = message.substring(i)
                        i = -1
                    }
                }
            }
            if (i >= message.length - 1 && startFlag >= 0) {
                message = socket.read()
            }
            i++
        }
    }

    private fun manageMessage(message: String) {
        with(objectMapper.readTree(message)) {
            val body = get("body")
            val status = get("status").asInt()
            val id: String? = get("id").asText(null)
            listeners.forEach { it.onNewMessage(status, id, body) }
        }
    }
}