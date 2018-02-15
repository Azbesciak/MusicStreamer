package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.UploadTokenRequest
import cs.sk.musicstreamer.utils.Informer
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

    fun sendFile(uploadData: UploadData, subscriber: UploadSubscriber) {
        connectionListeners.add(createUploadListener(uploadData, subscriber))
        connect()
    }

    private fun createUploadListener(uploadData: UploadData, subscriber: UploadSubscriber) =
            with(Informer()) {
                ConnectionListener(
                        onConnection = { initializeUploading(uploadData, subscriber) },
                        onError = {
                            ifNotInformed {
                                subscriber.onError(ErrorResponse(body = it.message ?: "Error while sending file"))
                            }
                        },
                        onDisconnect = connectionListeners::clear
                )
            }

    private fun Informer.initializeUploading(uploadData: UploadData, subscriber: UploadSubscriber) =
            send(UploadTokenRequest(uploadData.clientName, uploadData.token),
                    onResponse = {
                        launch {
                            try {
                                writer.write(
                                        file = uploadData.file,
                                        onProgress = subscriber.onProgress
                                )
                                ifNotInformed { subscriber.onSuccess() }

                            } catch (t: Throwable) {
                                ifNotInformed {
                                    subscriber.onError(ErrorResponse(body = "Could not send file"))
                                }
                            }
                            this@UploadConnector.disconnect()
                        }
                    },
                    onError = {
                        ifNotInformed { subscriber.onError(it) }
                    })
}

class UploadData(
        val file: File,
        val token: String,
        val clientName: String
)

class UploadSubscriber(
        val onProgress: (Double) -> Unit,
        val onSuccess: () -> Unit,
        val onError: (ErrorResponse) -> Unit
)