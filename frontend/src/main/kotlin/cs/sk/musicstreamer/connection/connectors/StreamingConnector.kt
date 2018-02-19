package cs.sk.musicstreamer.connection.connectors

import mu.KLogging
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.net.InetSocketAddress
import java.net.StandardProtocolFamily
import java.nio.channels.DatagramChannel
import java.util.*
import java.util.concurrent.atomic.AtomicBoolean

@Service
class StreamingConnector(
        @Value("\${server.streaming.port.min}") val minPort: Int,
        @Value("\${server.streaming.port.max}") val maxPort: Int,
        @Value("\${server.host}") val host: String,
        @Value("\${server.streamer.attentions}") val attentions: Int
) {
    private var port: Int? = null
    private val isConnected = AtomicBoolean(false)
    private var socket: DatagramChannel? = null

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

    @Synchronized
    fun close() {
        if (socket != null) {
            port = null
            isConnected.set(false)
            try {
                socket?.close()
                socket = null
            } catch (t: Throwable) {}
        }
    }

    fun getFrameListener() = ResponseListener(
            onResponse = {
                with(it.body) {
                    if (has("frameHeader")) {
                        logger.info { "Frame came: ${get("frameHeader").asText()}" }
                    }
                }
            }, onError = {logger.info { "error at broadcast..." }}
    )
}