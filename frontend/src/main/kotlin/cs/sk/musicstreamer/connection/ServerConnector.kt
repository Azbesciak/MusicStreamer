package cs.sk.musicstreamer.connection

import com.beust.klaxon.Klaxon
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
    }

    private val queue = ConcurrentLinkedQueue<ConnectionRequest>()
    private val isRunning = AtomicBoolean(true)
    private val serverDescription = "$host:$port"

    init {
        logger.info { "Loading server connector..." }
        launch {
            connect({ socket ->
                logger.info { "Successfully connected with $serverDescription" }
                while (isRunning.get()) {
                    queue.whileNotEmpty {
                        val wrote = socket.write(it.request)
                        if (wrote <= 0) {
                            logger.error { "Could not write to $serverDescription" }
                            it.onError(ErrorResponse(body = Error("Couldn't send request to server")))
                        } else {
                            socket.read(it.onResponse, it.onError)
                        }

                    }
                }
            }, {
                logger.error("Could not create connection with serverDescription", it)
            })
        }
    }


    private suspend fun connect(onSuccess: suspend (AsynchronousSocketChannel) -> Unit, onError: (Exception) -> Unit = {}) {
        try {
            AsynchronousSocketChannel.open().use {
                it.aConnect(InetSocketAddress(InetAddress.getByName(host), port))
                onSuccess(it)
            }
        } catch (e: Exception) {
            onError(e)
            shutdown() // just ok...
        }
    }

    fun shutdown() = isRunning.set(false)

    fun send(request: Request, onResponse: (Response<*>) -> Unit) =
            queue.add(ConnectionRequest(request, onResponse))

    private inline fun <T> ConcurrentLinkedQueue<T>.whileNotEmpty(f: (T) -> Unit) {
        while (isNotEmpty()) {
            f(poll())
        }
    }

    private suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = Klaxon().toJsonString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return aWrite(buf = bytes)
    }


    private suspend fun AsynchronousSocketChannel.read(
            onResponse: (Response<*>) -> Unit,
            onError: (ErrorResponse<*>) -> Unit) {
        val buffer = ByteBuffer.allocate(MAX_BUF_CAP)
        val read = aRead(buffer)
        if (read <= 0) {
            onError(ErrorResponse(500, Error("Error while reading response")))
        } else {
            val response = buffer.parse()
            when (response) {
                is ErrorResponse<*> -> onError(response)
                else -> onResponse(response)
            }
        }

    }

    private fun ByteBuffer.parse(): Response<*> {
        this.asCharBuffer().toString()
        return StringResponse(200, "OK")
    }

    data class ConnectionRequest(
            val request: Request,
            val onResponse: (Response<*>) -> Unit = {},
            val onError: (ErrorResponse<*>) -> Unit = {}
    )

}


