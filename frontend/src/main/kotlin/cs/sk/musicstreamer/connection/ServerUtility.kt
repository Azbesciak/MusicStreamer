package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import kotlinx.coroutines.experimental.launch
import kotlinx.coroutines.experimental.nio.aConnect
import kotlinx.coroutines.experimental.nio.aRead
import kotlinx.coroutines.experimental.nio.aWrite
import mu.KLogging
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.net.InetAddress
import java.net.InetSocketAddress
import java.nio.ByteBuffer
import java.nio.channels.AsynchronousSocketChannel
import java.util.concurrent.ConcurrentHashMap
import java.util.concurrent.CopyOnWriteArrayList
import java.util.concurrent.atomic.AtomicBoolean


@Service
class ReadWriteConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.connection.port}") port: Int) : Observable {

    companion object : KLogging()

    private val writer = SocketWriter()
    private val messagesListener = UndefinedMessageListener(
            listOf(ResponseListener(::manageUndefinedResponse, ::manageUndefinedResponse))
    )
    private val responseListener = IdentifiedMessageListener()
    private val reader = SocketReader(listOf(messagesListener, responseListener))
    final override val listeners = CopyOnWriteArrayList<ConnectionListener>()
    private val con = ServerCon(host, port, arrayOf(writer, reader), listeners)

    private val connectionListeners = mutableListOf(
            ConnectionListener(
                    { logger.info { "connection initialized" } },
                    { logger.info { "connection lost" } },
                    { logger.error("error with socket", it) }
            )
    )

    fun connect() = launch {
        con.connect()
    }

    fun isConnected() = con.isConnected()

    fun send(request: Request, onResponse: (JsonResponse) -> Unit, onError: (ErrorResponse) -> Unit) {
        launch {
            try {
                responseListener.waitForResponse(request, ResponseListener(onError = onError, onResponse = onResponse))
                writer.write(request)
            } catch (e: Error) {
                responseListener.removeListener(request)
                onError(ErrorResponse(500, "Error while managing request"))
                if (e is ConnectionError) {
                    connectionListeners.forEach { it.onError(e) }
                }
            }
        }
    }

    private fun manageUndefinedResponse(response: Response<*>) {
        logger.info { "Undefined response for ReadWriteConnector: $response" }
    }
}

interface Observable {
    val listeners: CopyOnWriteArrayList<ConnectionListener>
}

class ConnectionListener(
        val onConnection: () -> Unit = {},
        val onConnectionLost: () -> Unit = {},
        val onError: (Error) -> Unit = {}
)

@Service
class ReadingConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.broadcast.port}") port: Int
) : Observable {
    companion object : KLogging()

    private val messagesListener = UndefinedMessageListener()
    private val responseListener = IdentifiedMessageListener()
    private val reader = SocketReader(listOf(messagesListener, responseListener))
    final override val listeners = CopyOnWriteArrayList<ConnectionListener>()
    private val con = ServerCon(host, port, arrayOf(reader), listeners)
}

class ServerCon(host: String, port: Int,
                private val consumers: Array<SocketConsumer>,
                private val listeners: CopyOnWriteArrayList<ConnectionListener>) {

    companion object : KLogging()

    private val isConnected = AtomicBoolean(false)
    private val sock = Sock(host, port, isConnected)

    @Synchronized
    suspend fun connect() {
        if (!isConnected.get()) {
            try {
                sock.connect()
                consumers.forEach {
                    it.setSocket(sock)
                    it.setRunningFlag(isConnected)
                    it.start()
                }
                listeners.forEach { it.onConnection() }
            } catch (e: Throwable) {
                listeners.forEach { it.onError(Error(e)) }
            }
        }
    }

    fun isConnected() = isConnected.get()
}


class ConnectionError(reason: String) : Error(reason)

class Sock(
        private val host: String,
        private val port: Int,
        private val isConnected: AtomicBoolean = AtomicBoolean(false),
        private val bufferSize: Int = 256) {
    private var socket: AsynchronousSocketChannel? = null

    companion object : KLogging()

    val socketDescription = "$host:$port"

    suspend fun read() = withSocket { it.readSocket() }

    suspend fun write(request: Request) = withSocket { it.write(request) }

    private suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = ServerConnector.objectMapper.writeValueAsString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return aWrite(buf = bytes).apply {
            if (this < 0) throw ConnectionError("Could not write to $socketDescription")
        }
    }

    private suspend fun <T> withSocket(f: suspend (AsynchronousSocketChannel) -> T): T {
        if (socket == null) {
            throw ConnectionError("Socket is not connected")
        }
        return f(socket!!)
    }

    private fun ByteBuffer.asString() = String(array())

    private suspend fun AsynchronousSocketChannel.readSocket(): String {
        val buf = ByteBuffer.allocate(bufferSize)
        val read = aRead(buf)
        return if (read >= 0) buf.asString() else throw ConnectionError("Could not read from $socketDescription")
    }

    @Synchronized
    suspend fun connect() {
        if (!isConnected.get()) {
            try {
                socket = AsynchronousSocketChannel.open()
                logger.info { "Initializing connection..." }
                socket!!.aConnect(InetSocketAddress(InetAddress.getByName(host), port))
                logger.info { "Successfully connected with $socketDescription" }
                isConnected.set(true)
            } catch (e: Error) {
                close()
                logger.error("could not connect to $socketDescription", e)
                throw ConnectionError("Could not connect")
            }
        }
    }

    @Synchronized
    fun close() {
        if (socket != null) {
            try {
                socket!!.close()
            } catch (e: Error) {
            }
            socket = null
            isConnected.set(false)
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
                    else -> onResponse(JsonResponse(status, message, id))
                }
            }
}

class UndefinedMessageListener(
        listeners: List<ResponseListener> = listOf()
) : MessageListener {
    private val responseListeners = CopyOnWriteArrayList<ResponseListener>(listeners)

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
    override fun onClose() = with(responsesListeners) {
        forEach {
            it.value.onError(ErrorResponse(500, "Server closed"))
        }
        clear()
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

    fun removeListener(request: Request) {
        responsesListeners.remove(request.id)
    }
}

class ResponseListener(
        val onResponse: (JsonResponse) -> Unit,
        val onError: (ErrorResponse) -> Unit = {}
)

interface SocketConsumer {
    fun setSocket(socket: Sock)
    fun setRunningFlag(isRunning: AtomicBoolean)
    fun start()
}

class SocketWriter : SocketConsumer {
    private lateinit var sock: Sock
    private lateinit var isRunning: AtomicBoolean

    override fun setSocket(socket: Sock) {
        this.sock = socket
    }

    override fun setRunningFlag(isRunning: AtomicBoolean) {
        this.isRunning = isRunning
    }

    override fun start() {
    }

    suspend fun write(request: Request) = sock.write(request)

}

class SocketReader(
        private val listeners: List<MessageListener>
) : SocketConsumer {
    override fun setSocket(socket: Sock) {
        sock = socket
    }

    override fun setRunningFlag(isRunning: AtomicBoolean) {
        this.isRunning = isRunning
    }

    private lateinit var sock: Sock
    private lateinit var isRunning: AtomicBoolean
    private val isConnected = AtomicBoolean(false)

    companion object : KLogging() {
        val objectMapper = ObjectMapper()
    }

    override fun start() {
        if (!isConnected.getAndSet(true)) {
            launch {
                try {
                    while (isRunning.get()) {
                        consume(sock)
                    }
                } finally {
                    listeners.forEach { it.onClose() }
                    isConnected.set(false)
                }
            }
        }
    }

    private suspend fun consume(socket: Sock) {
        var startFlag = 0
        var startIndex = 0
        var i = 0
        var message = socket.read()
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
                        val subMessage = message.substring(startIndex, i + 1)
                        launch { manageMessage(subMessage) }
                        message = message.substring(i + 1)
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