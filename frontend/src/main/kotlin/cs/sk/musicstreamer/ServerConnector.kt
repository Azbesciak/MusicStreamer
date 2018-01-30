package cs.sk.musicstreamer

import com.beust.klaxon.Klaxon
import kotlinx.coroutines.experimental.launch
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
        @Value("\${server.host}")
        private val host: String,
        @Value("\${server.port}")
        private val port: Int) {

    companion object : KLogging()
    private val queue = ConcurrentLinkedQueue<ConnectionRequest>()
    private val isRunning = AtomicBoolean(true)

    init {
        logger.info { "INIT" }
        launch {
            connect({ socket ->
                while (isRunning.get()) {
                    queue.whileNotEmpty {
                        socket.write(it.request)

                    }
                }
            }, {
                logger.error("error with $host:$port", it)
            })
        }
    }

    private suspend fun connect(onSuccess: suspend (AsynchronousSocketChannel) -> Unit, onError: (Exception) -> Unit = {}) {
        try {
            AsynchronousSocketChannel.open().use {
                it.connect(InetSocketAddress(InetAddress.getByName(host), port))
                onSuccess(it)
            }
        } catch (e: Exception) {
            onError(e)
            isRunning.set(false) // just ok...
        }
    }

    fun close() = isRunning.set(false)

    fun send(request: Request, onResponse: (Response) -> Unit) =
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


    data class ConnectionRequest(
            val request: Request,
            val onResponse: (Response) -> Unit = {}
    )
}

