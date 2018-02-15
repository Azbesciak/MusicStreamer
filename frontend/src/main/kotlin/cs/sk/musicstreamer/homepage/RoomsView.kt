package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.*
import cs.sk.musicstreamer.connection.JoinRequest
import cs.sk.musicstreamer.connection.JsonResponse
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.MainConnector
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import io.datafx.controller.flow.context.ViewFlowContext
import javafx.beans.property.SimpleStringProperty
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
        private val broadCastConnector: BroadCastConnector,
        private val infoService: InfoService
) : Fragment() {

    override val root: StackPane by fxml("/main/rooms_view.fxml")
    private val roomsList: JFXListView<String> by fxid()
    private val joinInput: JFXTextField by fxid()
    private val joinButton: JFXRippler by fxid()
    private val currentRoom = SimpleStringProperty()
    private var roomsNamesList = mutableListOf<String>()
    private val joinListeners = mutableListOf<(String) -> Unit>()

    companion object : KLogging()

    @PostConstruct
    private fun bindListeners() {
        addOnRoomSelectListener()
        subscribeForRoomsBroadcast()
        addRoomCreateListeners()
    }

    private fun addRoomCreateListeners() {
        joinButton.disableProperty()
                .bind(with(joinInput.textProperty()) {
                    isEqualTo(currentRoom) or isEmpty
                })
        joinButton.setOnMouseClicked {
            val roomName = joinInput.textProperty().value
            joinToRoom(roomName) {
                joinInput.clear()
                addToRoomList(roomName)
                chooseRoomOnList(roomName)
            }
        }
    }

    private fun addOnRoomSelectListener() {
        roomsList.selectionModel.selectedItemProperty()
                .addListener { _, oldRoomName, newRoomName ->
                    when {
                        newRoomName != null &&
                                newRoomName != currentRoom.value -> joinToRoom(newRoomName)
                        oldRoomName != null &&
                                currentRoom.value != null &&
                                newRoomName == null &&
                                roomsNamesList.contains(oldRoomName) -> chooseRoomOnList(oldRoomName)
                    }
                }
    }

    private fun chooseRoomOnList(oldRoomName: String?) {
        roomsList.selectWhere { it == oldRoomName }
    }

    private fun joinToRoom(roomName: String, afterJoin: () -> Unit = {}) {
        logger.info { "joining to $roomName..." }
        mainConnector.send(
                request = JoinRequest(roomName),
                onResponse = {
                    launch(JavaFx) {
                        infoService.showSnackBar("joined to $roomName")
                        logger.debug { "Joined to room $it" }
                        currentRoom.value = roomName
                        joinListeners.forEach { it(roomName) }
                        afterJoin()
                    }
                },
                onError = {
                    launch(JavaFx) {
                        infoService.showSnackBar("Could not join to $roomName")
                        logger.info { "Could not join to $roomName" }
                    }
                }
        )
    }

    private fun subscribeForRoomsBroadcast() {
        broadCastConnector.addMessagesListener(ResponseListener(
                onResponse = { resp ->
                    launch(JavaFx) {
                        with(resp.body) {
                            if (has("rooms")) {
                                val rooms = get("rooms").map { it.asText() }
                                updateRooms(rooms)
                            }
                        }
                    }
                },
                onError = {}))
    }

    @Synchronized
    private fun updateRooms(rooms: List<String>) {
        roomsNamesList = rooms.toMutableList()
        roomsList.items.setAll(rooms)
    }

    fun clean() {
        leaveRoom()
        roomsNamesList.clear()
        roomsList.items.clear()
    }

    fun leaveRoom() {
        currentRoom.value = null
    }

    fun addJoinListener(listener: (room: String) -> Unit) = joinListeners.add(listener)

    private fun addToRoomList(roomName: String) {
        if (!roomsNamesList.contains(roomName)) {
            roomsNamesList.add(roomName)
            roomsList.items.add(roomName)
        }
    }
}