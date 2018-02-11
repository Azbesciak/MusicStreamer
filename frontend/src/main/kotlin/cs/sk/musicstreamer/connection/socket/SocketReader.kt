package cs.sk.musicstreamer.connection.socket

import com.fasterxml.jackson.databind.ObjectMapper
import cs.sk.musicstreamer.connection.listeners.MessageListener
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import java.util.concurrent.atomic.AtomicBoolean

class SocketReader(
        private val listeners: List<MessageListener>,
        private val onError: (Throwable) -> Unit
) : SocketConsumer {
    override fun setSocket(socket: SocketProxy) {
        socketProxy = socket
    }

    override fun setRunningFlag(isRunning: AtomicBoolean) {
        this.isRunning = isRunning
    }

    private lateinit var socketProxy: SocketProxy
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
                        consume(socketProxy)
                    }
                } catch (e: Throwable) {
                    onError(e)
                } finally {
                    listeners.forEach { it.onClose() }
                    isConnected.set(false)
                }
            }
        }
    }

    private suspend fun consume(socket: SocketProxy) {
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
            if (i >= message.length - 1 && startFlag > 0) {
                message = socket.read()
            }
            i++
        }
    }

    private fun manageMessage(message: String) {
        with(objectMapper.readTree(message)) {
            logger.debug { "New message from ${socketProxy.socketDescription}: $message" }
            val body = get("body")
            val status = get("status").asInt()
            val id: String? = if (hasNonNull("id")) get("id").asText() else null
            listeners.forEach { it.onNewMessage(status, id, body) }
        }
    }
}