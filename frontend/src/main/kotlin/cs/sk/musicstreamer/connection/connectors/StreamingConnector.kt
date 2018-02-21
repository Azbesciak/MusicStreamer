package cs.sk.musicstreamer.connection.connectors

import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.net.DatagramPacket
import java.net.InetSocketAddress
import java.net.StandardProtocolFamily
import java.nio.ByteBuffer
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

    private lateinit var listener: StreamingListener
    private var receiveBuff = ByteBuffer.allocate(8096)

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
            launch {
                if (!isListening.getAndSet(true)) {
                    while (isListening.get()) {
                        receiveBuff.clear()
                        socket!!.receive(receiveBuff)
                        if (isListening.get()) {
                            listener.onNewData(receiveBuff.array())
                        }
                    }
                }
            }
        } catch (t: Throwable) {
            listener.onError(t)
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
//                        listener.formatListener(AudioFormat())
                        logger.info { "Frame came: ${get("frameHeader").asText()}" }
                    }
                }
            }, onError = { logger.info { "error at broadcast..." } }
    )

    fun registerListener(listener: StreamingListener) {
        this.listener = listener
        listener.switch(this)
    }
}

interface Switchable {
    fun stop()
    fun start()
}

class StreamingListener(
        val formatListener: (AudioFormat) -> Unit,
        val switch: (Switchable) -> Unit,
        val onNewData: (ByteArray) -> Unit,
        val onError: (Throwable) -> Unit
)