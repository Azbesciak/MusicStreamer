package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXListView
import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.connection.JoinRequest
import cs.sk.musicstreamer.connection.ReadWriteConnector
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
        private val readWriteConnector: ReadWriteConnector,
        private val viewContext: ViewFlowContext
) : Fragment() {

    override val root: StackPane by fxml("/main/rooms_view.fxml")
    private val roomsList: JFXListView<String> by fxid()
    private val snackBar: JFXSnackbar by lazy {viewContext.getRegisteredObject(JFXSnackbar::class.java) }

    companion object : KLogging()

    @PostConstruct
    fun bindListeners() {
        roomsList.selectionModel.selectedItemProperty().addListener { _, _, roomName ->
            readWriteConnector.send(
                    request = JoinRequest(roomName),
                    onResponse = {
                        launch(JavaFx) {
                            snackBar.fireEvent(JFXSnackbar.SnackbarEvent("joined to $roomName"))
                            logger.debug { "Joined to room $roomName" }
                        }
                    },
                    onError = {}
            )
        }
    }

    fun updateRooms(rooms: List<String>) {
        roomsList.items.setAll(rooms)
    }


}