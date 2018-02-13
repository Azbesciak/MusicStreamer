package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.UploadTokenRequest
import kotlinx.coroutines.experimental.launch
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

    fun sendFile(uploadData: UploadData, listener: UploadListener) {
        connectionListeners.add(ConnectionListener(
                onConnection = {
                    send(UploadTokenRequest(uploadData.clientName, uploadData.token),
                            onResponse = {
                                launch {
                                    try {
                                        writer.write(
                                                file = uploadData.file,
                                                onProgress = listener.onProgress
                                        )
                                        listener.onSuccess()
                                    } catch (t: Throwable) {
                                        listener.onError(ErrorResponse(body = "Could not send file"))
                                    }
                                    this@UploadConnector.disconnect()
                                }
                            },
                            onError = {
                                listener.onError(it)
                            })
                },
                onError = {
                    listener.onError(ErrorResponse(body = it.message ?: "Error while sending file"))
                },
                onDisconnect = connectionListeners::clear
        ))
        connect()
    }
}

class UploadData(
        val file: File,
        val token: String,
        val clientName: String
)

class UploadListener(
        val onProgress: (Double) -> Unit,
        val onSuccess: () -> Unit,
        val onError: (ErrorResponse) -> Unit
)