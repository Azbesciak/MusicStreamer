package cs.sk.musicstreamer.connection.listeners

import com.fasterxml.jackson.databind.JsonNode
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import java.util.concurrent.CopyOnWriteArraySet

class UndefinedMessageListener(
        listeners: List<ResponseListener> = listOf()
) : MessageListener {
    private val responseListeners = CopyOnWriteArraySet<ResponseListener>(listeners)

    override fun onClose() {}

    override fun onNewMessage(status: Int, id: String?, message: JsonNode) {
        if (id == null) {
            responseListeners.forEach {
                manage(status, id, message, it)
            }
        }
    }

    fun registerListener(listener: ResponseListener) {
        responseListeners += listener
    }
}