package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXListView
import javafx.scene.layout.StackPane
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class RoomsView : Fragment() {
    override val root: StackPane by fxml("/main/rooms_view.fxml")
    private val roomsList: JFXListView<String> by fxid()

    @PostConstruct
    fun bindListeners() {
        roomsList.selectionModel.selectedItemProperty().addListener {
            observable, oldValue, newValue -> print(newValue)
        }
    }

    fun updateRooms(rooms: List<String>) {
        roomsList.items.setAll(rooms)
    }
}