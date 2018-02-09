package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.JsonNode
import com.fasterxml.jackson.databind.ObjectMapper
import kotlinx.coroutines.experimental.delay
import kotlinx.coroutines.experimental.launch
import kotlinx.coroutines.experimental.nio.aConnect
import kotlinx.coroutines.experimental.nio.aRead
import kotlinx.coroutines.experimental.nio.aWrite
import mu.KLogging
import java.net.InetAddress
import java.net.InetSocketAddress
import java.nio.ByteBuffer
import java.nio.channels.AsynchronousSocketChannel
import java.util.concurrent.atomic.AtomicBoolean

abstract class ServerConnector(
        private val host: String,
        private val port: Int,
        private val name: String) {


    companion object : KLogging() {
        const val MAX_BUF_CAP = 512
        val objectMapper = ObjectMapper()
    }


    private val isRunning = AtomicBoolean(false)
    protected val serverDescription = "$host:$port"

    fun initialize() =
            initializeConnection(
                    { logger.info { "Successfully connected $name with $serverDescription" } },
                    { logger.error("Could not create connection for $name with $serverDescription", it) }
            )

    abstract suspend fun onConnection(socket: AsynchronousSocketChannel)
    abstract fun parseResponse(status: Int, body: JsonNode): Response<*>

    fun initializeConnection(onSuccess: (wasConnected: Boolean) -> Unit, onError: (Exception) -> Unit) {
        if (isRunning.get().not()) {
            logger.info { "Initializing connection" }
            launch {
                connect({ socket ->
                    onSuccess(false)
                    while (isRunning.get()) {
                        onConnection(socket)
                        delay(1000)
                    }
                }, onError = onError)
            }
        } else {
            onSuccess(true)
        }
    }

    fun isRunning() = isRunning.get()

    private suspend fun connect(onSuccess: suspend (AsynchronousSocketChannel) -> Unit, onError: (Exception) -> Unit = {}) {
        try {
            isRunning.set(true)
            AsynchronousSocketChannel.open().use {
                it.aConnect(InetSocketAddress(InetAddress.getByName(host), port))
                onSuccess(it)
            }
        } catch (e: Exception) {
            onError(e)
        }
        shutdown() // just ok...
    }

    fun shutdown() = isRunning.set(false)

    protected suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = objectMapper.writeValueAsString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return aWrite(buf = bytes)
    }


    protected suspend fun AsynchronousSocketChannel.read(
            onResponse: (Response<*>) -> Unit,
            onError: (ErrorResponse) -> Unit) {
        val buffer = ByteBuffer.allocate(MAX_BUF_CAP)
        val read = aRead(buffer)
        if (read <= 0) {
            onError(ErrorResponse(500, "Error while reading response"))
        } else {
            val response = buffer.parse()
            when (response) {
                is ErrorResponse -> onError(response)
                else -> onResponse(response)
            }
        }
    }

    fun defaultResponse(status: Int, body: JsonNode) = StringResponse(status, body.asText())

    protected fun ByteBuffer.parse(): Response<*> {
        with(objectMapper.readTree(String(array()))) {
            val body = get("body")
            val status = get("status").asInt()
            val idNode = get("id")
            hasNonNull("id")
            return when {
                status in 400..499 -> ErrorResponse(status, body.get("error").asText())
                status >= 500 -> ErrorResponse(status, "Internal server error")
                        .also { logger.error { body.get("error") } }
                else -> parseResponse(status, body)
            }
        }
    }

    protected data class ConnectionRequest(
            val request: Request,
            val onResponse: (Response<*>) -> Unit = {},
            val onError: (ErrorResponse) -> Unit = {}
    )
}
