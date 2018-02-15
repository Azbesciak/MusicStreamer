package cs.sk.musicstreamer.room

import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.UploadRequest
import cs.sk.musicstreamer.connection.connectors.MainConnector
import cs.sk.musicstreamer.connection.connectors.UploadConnector
import cs.sk.musicstreamer.connection.connectors.UploadData
import cs.sk.musicstreamer.connection.connectors.UploadSubscriber
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.springframework.stereotype.Service
import java.io.File

@Service
class MusicUploadService(
        private val mainConnector: MainConnector,
        private val converter: MusicConverter,
        private val uploadConnector: UploadConnector,
        private val authService: AuthService
) {

    companion object : KLogging()

    fun upload(file: File, subscriber: UploadSubscriber) {
        launch {
            val fileToUpload = when (file.extension.toLowerCase()) {
                "mp3" -> converter.mp3ToWav(file)
                "wav" -> file
                else -> throw UnsupportedOperationException("File extension not supported")
            }
            val trackName = file.nameWithoutExtension
            val fileSize = fileToUpload.length()
            logger.info { "File to upload: $trackName $fileToUpload" }
            mainConnector.send(
                    request = UploadRequest(trackName, fileSize),
                    onResponse = {
                        logger.info { "upload ok ! $it" }
                        val clientName = authService.getUserName() ?: return@send
                        uploadConnector.sendFile(
                                uploadData = UploadData(fileToUpload, it.body.get("uploadToken").asText(), clientName),
                                subscriber = subscriber
                        )
                    },
                    onError = { logger.error { "Upload denied" }
                        subscriber.onError(it)
                    }
            )
        }
    }

    fun cancelUpload() = uploadConnector.disconnect()
}