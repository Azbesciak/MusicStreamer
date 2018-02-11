package cs.sk.musicstreamer.connection.listeners

import com.fasterxml.jackson.databind.JsonNode
import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.Request
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import mu.KLogging
import java.util.concurrent.ConcurrentHashMap

class IdentifiedMessageListener : MessageListener {
    companion object : KLogging()

    private val responsesListeners = ConcurrentHashMap<String, ResponseListener>()
    override fun onClose() = with(responsesListeners) {
        forEach {
            it.value.onError(ErrorResponse(500, "Server closed"))
        }
        clear()
    }

    override fun onNewMessage(status: Int, id: String?, message: JsonNode) {
        id?.let {
            responsesListeners[id]?.let {
                manage(status, id, message, it)
                responsesListeners.remove(id)
            } ?: logger.error { "listener for request $id was not found!" }
        }
    }

    fun waitForResponse(request: Request, listener: ResponseListener) =
            responsesListeners.put(request.id, listener)

    fun removeListener(request: Request) {
        responsesListeners.remove(request.id)
    }
}