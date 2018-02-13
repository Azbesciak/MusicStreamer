package cs.sk.musicstreamer.connection.socket

import com.fasterxml.jackson.databind.ObjectMapper
import cs.sk.musicstreamer.connection.connectors.ConnectionError
import cs.sk.musicstreamer.connection.Request
import kotlinx.coroutines.experimental.nio.aConnect
import kotlinx.coroutines.experimental.nio.aRead
import kotlinx.coroutines.experimental.nio.aWrite
import mu.KLogging
import java.io.File
import java.net.InetAddress
import java.net.InetSocketAddress
import java.nio.ByteBuffer
import java.nio.channels.AsynchronousFileChannel
import java.nio.channels.AsynchronousSocketChannel
import java.nio.file.StandardOpenOption
import java.util.concurrent.atomic.AtomicBoolean

class SocketProxy(
        private val host: String,
        private val port: Int,
        private val isConnected: AtomicBoolean = AtomicBoolean(false),
        private val bufferSize: Int = 256,
        private val fileBufferSize: Int = 1024) {

    companion object : KLogging() {
        val objectMapper = ObjectMapper()
    }

    private var socket: AsynchronousSocketChannel? = null
    val socketDescription = "$host:$port"

    suspend fun read() = withSocket { it.readSocket() }

    suspend fun write(request: Request) = withSocket { it.write(request) }

    suspend fun write(file: File, progressListener: (Double) -> Unit) =
            withSocket {it.write(file, progressListener)}

    private suspend fun AsynchronousSocketChannel.write(file: File, progressListener: (Double) -> Unit) {
        val buf = ByteBuffer.allocate(fileBufferSize)
        AsynchronousFileChannel.open(file.toPath(), StandardOpenOption.READ).use { chanel ->
            var position = 0L
            val totalSize = file.length()
            while (position < totalSize) {
                val read = chanel.aRead(buf, position)
                if (read <= 0) {
                    throwAndClose("Could not write file")
                } else {
                    buf.flip()
                    writeOrThrow(buf)
                    position += read
                    progressListener(position / totalSize.toDouble())
                }
                buf.clear()
            }
        }
    }

    private suspend fun AsynchronousSocketChannel.write(request: Request): Int {
        val requestString = objectMapper.writeValueAsString(request)
        val bytes = ByteBuffer.wrap(requestString.toByteArray())
        return writeOrThrow(buf = bytes)
    }

    private suspend fun AsynchronousSocketChannel.writeOrThrow(buf: ByteBuffer): Int =
            aWrite(buf = buf).apply {
                if (this <= 0) throwAndClose("Could not write to $socketDescription")
            }

    private suspend fun <T> withSocket(f: suspend (AsynchronousSocketChannel) -> T): T {
        if (socket == null) {
            throwAndClose("Socket is not connected")
        }
        return f(socket!!)
    }

    private fun ByteBuffer.asString() = String(array())

    private suspend fun AsynchronousSocketChannel.readSocket(): String {
        val buf = ByteBuffer.allocate(bufferSize)
        val read = aRead(buf)
        return if (read >= 0)
            buf.asString().replace("\u0000", "")
        else
            throwAndClose("Could not read from $socketDescription")
    }

    private fun throwAndClose(message: String): Nothing = throw ConnectionError(message)
            .also { close() }

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
    fun close(): Boolean {
        isConnected.set(false)
        if (socket != null) {
            try {
                socket?.close()
            } catch (e: Error) {
            }
            socket = null
            return true
        }
        return false
    }
}