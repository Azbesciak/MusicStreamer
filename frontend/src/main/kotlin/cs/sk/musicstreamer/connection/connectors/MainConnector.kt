package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.connectors.ConnectionListener
import cs.sk.musicstreamer.connection.connectors.Connector
import cs.sk.musicstreamer.connection.Response
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service

@Service
class MainConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.connection.port}") port: Int) : Connector(host, port) {

    override fun initialConnectionListeners() =
            listOf(ConnectionListener(
                    { logger.info { "connection initialized" } },
                    { logger.info { "disconnected with read-write server" } },
                    { logger.error("error with socket", it) }
            ))

    override fun initialMessagesListeners() = listOf(ResponseListener(::manageUndefinedResponse, ::manageUndefinedResponse))

    private fun manageUndefinedResponse(response: Response<*>) {
        logger.info { "Undefined response for MainConnector: $response" }
    }
}