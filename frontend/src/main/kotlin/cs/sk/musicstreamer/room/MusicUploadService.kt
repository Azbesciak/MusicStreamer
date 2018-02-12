package cs.sk.musicstreamer.room

import cs.sk.musicstreamer.connection.UploadRequest
import cs.sk.musicstreamer.connection.connectors.MainConnector
import cs.sk.musicstreamer.connection.connectors.UploadConnector
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.reactivestreams.Subscriber
import org.springframework.stereotype.Service
import java.io.File

@Service
class MusicUploadService(
        private val mainConnector: MainConnector,
        private val converter: MusicConverter,
        private val uploadConnector: UploadConnector
) {

    companion object : KLogging()

    infix fun upload(file: File) {
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
                    onResponse = { logger.info { "upload ok ! $it" } },
                    onError = { logger.error { "Upload denied" } }
            )
        }
    }
}