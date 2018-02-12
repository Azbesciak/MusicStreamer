package cs.sk.musicstreamer.connection.connectors

import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.io.File


@Service
class UploadConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.upload.port}") port: Int
) : Connector(host, port) {
    override fun initialConnectionListeners() =
            listOf(ConnectionListener(
                    { logger.info { "connection initialized" } },
                    { logger.info { "disconnected with read-write server" } },
                    { logger.error("error with socket", it) }
            ))

    fun sendFile(file: File, token: String, listener: UploadListener) {
        connect() // TODO.

    }
}

class UploadListener(
        val onProgress: (Double) -> Unit,
        val onSuccess: () -> Unit,
        val onError: (Error) -> Unit
)