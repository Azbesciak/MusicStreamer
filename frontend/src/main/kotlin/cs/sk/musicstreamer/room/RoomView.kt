package cs.sk.musicstreamer.room

import com.jfoenix.controls.JFXListView
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import cs.sk.musicstreamer.homepage.AppLabel
import javafx.scene.control.Label
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class RoomView(
        private val broadCastConnector: BroadCastConnector,
        private val appLabel: AppLabel
) : View() {
    override val root: StackPane by fxml("/main/room_view.fxml")
    private val clients: JFXListView<String> by fxid()
    private val tracks: JFXListView<String> by fxid()

    @PostConstruct
    private fun bind() {
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
    }

    private fun updateState(roomName: String = "", clients: List<String> = listOf()) {
        launch(JavaFx) {
            appLabel.setRoom(roomName)
            this@RoomView.clients.items.setAll(clients)
        }
    }

    fun clean() {
        updateState()
    }
}