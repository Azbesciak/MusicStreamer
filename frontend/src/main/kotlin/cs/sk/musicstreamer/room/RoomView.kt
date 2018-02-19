package cs.sk.musicstreamer.room

import com.fasterxml.jackson.databind.JsonNode
import com.jfoenix.controls.JFXButton
import com.jfoenix.controls.JFXListView
import com.jfoenix.controls.JFXProgressBar
import cs.sk.musicstreamer.connection.ErrorResponse
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import cs.sk.musicstreamer.connection.connectors.UploadSubscriber
import cs.sk.musicstreamer.homepage.AppLabel
import cs.sk.musicstreamer.homepage.InfoService
import cs.sk.musicstreamer.utils.getStrings
import javafx.beans.property.SimpleBooleanProperty
import javafx.scene.layout.StackPane
import javafx.stage.FileChooser
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class RoomView(
        private val broadCastConnector: BroadCastConnector,
        private val appLabel: AppLabel,
        private val musicUploadService: MusicUploadService,
        private val infoService: InfoService
) : View() {
    override val root: StackPane by fxml("/main/room_view.fxml")
    private val uploadButton: JFXButton by fxid()
    private val uploadProgress: JFXProgressBar by fxid()
    private val clients: JFXListView<String> by fxid()
    private val tracks: JFXListView<String> by fxid()
    private val isRoomSet = SimpleBooleanProperty(false)
    private val isUploading = SimpleBooleanProperty(false)
    private var currentRoomName: String? = null;
    companion object : KLogging()

    @PostConstruct
    private fun bind() {
        resetUpload()
        broadCastConnector.addMessagesListener(
                ResponseListener(onResponse = {
                        it.body.onBroadCastMessage()
                }
                )
        )
        with(clients) {
            isExpanded = true
            depth = 1
        }

        initializeUploadButton()
    }

    private fun JsonNode.onBroadCastMessage() {
        if (has("room")) {
            val roomName = get("room").asText()
            if (roomName == currentRoomName)
                when { //TODO what with room... can be not synchronized, timestamp?
                    has("clients") -> updateState(roomName, getStrings("clients"))
                    has("tracks") -> updateTracks(getStrings("tracks"))
                }
        }
    }

    private fun initializeUploadButton() {
        uploadButton.setOnMouseClicked {
            isUploading.value = true
            val filesToUpload = chooseFile(
                    title = "Choose track to upload",
                    filters = arrayOf(FileChooser.ExtensionFilter("music files (wav, mp3)", "*.wav", "*.mp3"))
            )
            if (filesToUpload.isEmpty()) {
                isUploading.value = false
            }
            filesToUpload.forEach {
                uploadProgress.show()
                musicUploadService.upload(it, UploadSubscriber(
                        onProgress = {
                            uploadProgress.progress = it
                            logger.debug { "progress: $it" }
                        },
                        onError = {
                            resetUpload()
                            onUploadFailure(it)
                        },
                        onSuccess = {
                            resetUpload()
                            infoService.showSnackBar("File uploaded with success")
                            logger.info { "file uploaded." }
                        }
                ))
            }
        }
        uploadButton.disableWhen { isRoomSet.not().or(isUploading) }
    }

    fun onUploadFailure(errorResp: ErrorResponse) {
        var message = "Could not upload file"
        if (!errorResp.isServerError()) {
            message += ": ${errorResp.body}"
        }
        infoService.showSnackBar(message)
        logger.error { "Error at file upload $errorResp" }
    }

    private fun resetUpload() {
        uploadProgress.hide()
        uploadProgress.progress = 0.0
        isUploading.value = false
    }

    private fun updateTracks(tracks: List<String> = listOf()) {
        this.tracks.items.setAll(tracks)
    }


    fun pressingRoomName(roomName: String? = null) {
        currentRoomName = roomName
    }

    private fun updateState(roomName: String? = null, clients: List<String> = listOf()) {
        launch(JavaFx) {
            if (roomName != null) {
                appLabel.setRoom(roomName)
                isRoomSet.value = true
            } else {
                appLabel.clean()
                isRoomSet.value = true
            }
            this@RoomView.clients.items.setAll(clients)
        }
    }

    fun clean() {
        updateState()
        updateTracks()
        pressingRoomName()
        musicUploadService.cancelUpload()
    }
}