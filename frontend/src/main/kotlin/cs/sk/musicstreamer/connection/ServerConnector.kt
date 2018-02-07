package cs.sk.musicstreamer.connection

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
import java.util.concurrent.ConcurrentLinkedQueue
import java.util.concurrent.atomic.AtomicBoolean


@Service
class ServerConnector(
        @Value("\${server.host}") private val host: String,
        @Value("\${server.port}") private val port: Int
) {

    companion object : KLogging() {
        const val MAX_BUF_CAP = 512
        val objectMapper = ObjectMapper()
    }

    private val queue = ConcurrentLinkedQueue<ConnectionRequest>()
    private val isRunning = AtomicBoolean(false)
    private val serverDescription = "$host:$port"

    init {
        initializeConnection(
                { logger.info { "Successfully connected with $serverDescription" } },
                { logger.error("Could not create connection with $serverDescription", it) }
        )
    }

    final fun initializeConnection(onSuccess: (Boolean) -> Unit, onError: (Exception) -> Unit) {
        if (isRunning.get().not()) {
            logger.info { "Initializing connection" }
            launch {
                connect({ socket ->
                    onSuccess(false)
                    while (isRunning.get()) {
                        queue.whileNotEmpty {
                            val wrote = socket.write(it.request)
                            if (wrote <= 0) {
                                logger.error { "Could not write to $serverDescription" }
                                it.onError(ErrorResponse(body = "Couldn't send request to server"))
                            } else {
                                socket.read(it.onResponse, it.onError)
                            }
                        }
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

    fun send(request: Request, onResponse: (Response<*>) -> Unit, onError: (ErrorResponse) -> Unit) =
            queue.add(ConnectionRequest(request, onResponse, onError))

    private inline fun <T> ConcurrentLinkedQueue<T>.whileNotEmpty(f: (T) -> Unit) {
        while (isNotEmpty()) {
            f(poll())
        }
    }

    private suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = objectMapper.writeValueAsString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return aWrite(buf = bytes)
    }


    private suspend fun AsynchronousSocketChannel.read(
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

    private fun ByteBuffer.parse(): Response<*> {
        with(objectMapper.readTree(String(array()))) {
            val body = get("body")
            val status = get("status").asInt()
            return when {
                status in 400..499 -> ErrorResponse(status, body.get("error").asText())
                status >= 500 -> ErrorResponse(status, "Internal server error")
                        .also { logger.error { body.get("error") } }
                else -> StringResponse(status, body.asText())
            }
        }
    }

    private data class ConnectionRequest(
            val request: Request,
            val onResponse: (Response<*>) -> Unit = {},
            val onError: (ErrorResponse) -> Unit = {}
    )
}


