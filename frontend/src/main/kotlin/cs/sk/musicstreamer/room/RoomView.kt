package cs.sk.musicstreamer.room

import com.jfoenix.controls.JFXButton
import com.jfoenix.controls.JFXListView
import com.jfoenix.controls.JFXProgressBar
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import cs.sk.musicstreamer.connection.connectors.UploadListener
import cs.sk.musicstreamer.homepage.AppLabel
import cs.sk.musicstreamer.homepage.InfoService
import javafx.scene.control.Label
import javafx.scene.layout.StackPane
import javafx.stage.FileChooser
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
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

    @PostConstruct
    private fun bind() {
        resetProgress()
        broadCastConnector.addMessagesListener(
                ResponseListener(
                        onResponse = {
                            with(it.body) {
                                if (has("clients") && has("room")) {
                                    updateState(get("room").asText(), get("clients").map { it.asText() })
                                }
                            }
                        },
                        onError = {

                        }
                )
        )
        with (clients) {
            isExpanded = true
            depth = 1
        }

        uploadButton.setOnMouseClicked {
            val filesToUpload = chooseFile(
                    title = "Choose track to upload",
                    filters = arrayOf(FileChooser.ExtensionFilter("music files (wav, mp3)", "*.wav", "*.mp3"))
            )
            filesToUpload.forEach {
                uploadProgress.show()
                musicUploadService.upload(it,  UploadListener(
                        onProgress = {
                            uploadProgress.progress = it
//                            MusicUploadService.logger.info { "progress: $it" }
                        },
                        onError = {
                            resetProgress()
                            infoService.showSnackBar("Could not upload file")
                            MusicUploadService.logger.error { "Error at file upload $it" }
                        },
                        onSuccess = {
                            resetProgress()
                            infoService.showSnackBar("File uploaded with success")
                            MusicUploadService.logger.info { "file uploaded." }
                        }
                ))
            }
        }
    }

    private fun resetProgress() {
        uploadProgress.hide()
        uploadProgress.progress = 0.0
    }

    private fun updateState(roomName: String? = null, clients: List<String> = listOf()) {
        launch(JavaFx) {
            if (roomName != null)
                appLabel.setRoom(roomName)
            else
                appLabel.clean()
            this@RoomView.clients.items.setAll(clients)
        }
    }

    fun clean() {
        updateState()
    }
}