package cs.sk.musicstreamer.connection.connectors

import mu.KLogging
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.net.DatagramPacket
import java.net.InetSocketAddress
import java.net.StandardProtocolFamily
import java.nio.channels.DatagramChannel
import java.util.*
import java.util.concurrent.atomic.AtomicBoolean
import javax.sound.sampled.AudioFormat

@Service
class StreamingConnector(
        @Value("\${server.streaming.port.min}") val minPort: Int,
        @Value("\${server.streaming.port.max}") val maxPort: Int,
        @Value("\${server.host}") val host: String,
        @Value("\${server.streamer.attentions}") val attentions: Int
) : Switchable {
    override fun stop() {
        isListening.set(false)
    }

    override fun start() {
        listen()
    }

    private var port: Int? = null
    private val isConnected = AtomicBoolean(false)
    private var socket: DatagramChannel? = null

    private var isListening = AtomicBoolean(false)

    private lateinit var formatListener: (AudioFormat) -> Unit
    private var tempBuffer = ByteArray(10000)

    companion object : KLogging() {
        val rand = Random()
    }

    @Synchronized
    fun connect(): Int? {
        if (!isConnected.get()) {
            socket = DatagramChannel.open(StandardProtocolFamily.INET)
            var attentionsLeft = attentions
            while (attentionsLeft-- > 0) {
                try {
                    port = rand.nextInt(maxPort - minPort) + minPort
                    socket!!.connect(InetSocketAddress(host, port!!))
                    isConnected.set(true)
                    return port
                } catch (t: Throwable) {
                    close()
                }
            }
        }
        return port
    }

    private fun listen() {
        try {
            if (!isListening.getAndSet(true)) {
//                socket!!.
//                val receivePacket = DatagramPacket(receiveData, receiveData.size)
//                serverSocket.receive(receivePacket)
            }
        } catch (t: Throwable) {

        }
    }

    @Synchronized
    fun close() {
        if (socket != null) {
            port = null
            isConnected.set(false)
            try {
                socket?.close()
                socket = null
            } catch (t: Throwable) {
            }
        }
    }

    fun getFrameListener() = ResponseListener(
            onResponse = {
                with(it.body) {
                    if (has("frameHeader")) {
                        logger.info { "Frame came: ${get("frameHeader").asText()}" }
                    }
                }
            }, onError = { logger.info { "error at broadcast..." } }
    )
}

interface Switchable {
    fun stop()
    fun start()
}

class StreamingListener(
        val formatListener: (AudioFormat) -> Unit,
        val switch: (Switchable) -> Unit,
        val onNewData: (ByteArray) -> Unit,
        val onError: (Error) -> Unit
)