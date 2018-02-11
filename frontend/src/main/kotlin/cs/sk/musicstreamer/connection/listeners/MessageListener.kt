package cs.sk.musicstreamer.connection.listeners

import com.fasterxml.jackson.databind.JsonNode
import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.JsonResponse
import cs.sk.musicstreamer.connection.connectors.ResponseListener

interface MessageListener {
    fun onNewMessage(status: Int, id: String?, message: JsonNode)
    fun onClose()
    fun manage(status: Int, id: String?, message: JsonNode, listener: ResponseListener) =
            with(listener) {
                when {
                    status in 400..499 -> onError(ErrorResponse(status, message.get("error").asText()))
                    status >= 500 -> onError(ErrorResponse(status, "Internal server error"))
                    else -> onResponse(JsonResponse(status, message, id))
                }
            }
}