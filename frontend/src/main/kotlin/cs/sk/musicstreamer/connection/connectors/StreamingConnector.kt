package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.JsonResponse
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

    companion object : KLogging() {
        val rand = Random()
    }

    private var port: Int? = null
    private val isConnected = AtomicBoolean(false)
    private var socket: DatagramChannel? = null

    private val isListening = AtomicBoolean(false)
    private val isRunning = AtomicBoolean(false)

    private lateinit var listener: StreamingListener
    private var receiveBuff = ByteBuffer.allocate(8096)

    override fun stop() {
        isListening.set(false)
        logger.info { "Requested Streaming connector to stop..." }
    }

    override fun start() {
        listen()
        logger.info { "Streaming connector is running!" }
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
        launch {
            try {
                logger.info { "in listen..." }
                when {
                    isRunning.get() -> isListening.set(true)
                    !isListening.getAndSet(true) -> {
                        isRunning.set(true)
                        while (isListening.get()) {
                            receiveBuff.clear()
                            socket?.receive(receiveBuff)?.let {
                                logger.info { "data came!" }
                                if (isListening.get()) {
                                    listener.onNewData(receiveBuff.array())
                                }
                            } ?: let {
                                isListening.set(false)
                                listener.onError(ConnectionError("No Connection at streamer"))
                            }
                        }
                        isRunning.set(false)
                        logger.info { "Streamer stopped working" }
                    }
                    else -> { logger.info { "ELSE" }}
                }
            } catch (t: Throwable) {
                listener.onError(t)
                isRunning.set(false)
                isListening.set(false)
            }
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
                    parseAudioFormat(it)?.let {
                        listener.formatListener(it)
                    }
                }
            }, onError = { logger.info { "error at broadcast..." } }
    )

    private fun parseAudioFormat(json: JsonResponse): AudioFormat? {

        with(json.body) {

            if (has("sampleRate") and has("bitsPerSample") and has("channels")) {

                val sampleRate = get("sampleRate").asInt()
                val bitsPerSample = get("bitsPerSample").asInt()
                val channels = get("channels").asInt()

                return AudioFormat(sampleRate.toFloat(), bitsPerSample, channels, true, false)
            }
        }

        return null
    }

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