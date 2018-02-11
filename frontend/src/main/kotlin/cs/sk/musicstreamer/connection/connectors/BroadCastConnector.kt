package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.connectors.ResponseListener
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service

@Service
class BroadCastConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.broadcast.port}") port: Int
) : Connector(host, port) {
    fun addMessagesListener(listener: ResponseListener) = messagesListener.registerListener(listener)
}