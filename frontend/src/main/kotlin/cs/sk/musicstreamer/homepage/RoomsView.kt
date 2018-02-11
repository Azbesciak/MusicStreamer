package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXListView
import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.connection.JoinRequest
import cs.sk.musicstreamer.connection.JsonResponse
import cs.sk.musicstreamer.connection.connectors.MainConnector
import io.datafx.controller.flow.context.ViewFlowContext
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class RoomsView(
        private val mainConnector: MainConnector,
        private val viewContext: ViewFlowContext
) : Fragment() {

    override val root: StackPane by fxml("/main/rooms_view.fxml")
    private val roomsList: JFXListView<String> by fxid()
    private val snackBar: JFXSnackbar by lazy { viewContext.getRegisteredObject(JFXSnackbar::class.java) }
    companion object : KLogging()

    @PostConstruct
    fun bindListeners() {
        var currentRoom: String? = null
        roomsList.selectionModel.selectedItemProperty().addListener { _, oldRoomName, newRoomName ->
            if (newRoomName != null && newRoomName != currentRoom) {
                logger.info { "joining to $newRoomName..." }
                mainConnector.send(
                        request = JoinRequest(newRoomName),
                        onResponse = {
                            launch(JavaFx) {
                                snackBar.fireEvent(JFXSnackbar.SnackbarEvent("joined to $newRoomName"))
                                logger.debug { "Joined to room $it" }
                                currentRoom = newRoomName
                            }
                        },
                        onError = {}
                )
            } else if (oldRoomName != null) {
//                what if i want to leave...?
                roomsList.selectWhere { it == oldRoomName }
            }
        }
    }

    @Synchronized
    fun updateRooms(rooms: List<String>) {
        roomsList.items.setAll(rooms)
    }


    class Room(val roomName: String, val clients: List<String>) {
        companion object {
            fun fromResponse(response: JsonResponse) =
                    with(response.body) {
                        Room(get("room").asText(), get("clients").map { it.asText() })
                    }

        }
    }

}