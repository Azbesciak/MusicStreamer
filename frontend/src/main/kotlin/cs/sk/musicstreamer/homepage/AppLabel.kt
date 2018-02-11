package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXRippler
import javafx.scene.control.Label
import javafx.scene.layout.HBox
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class AppLabel: Fragment() {
    override val root: HBox by fxml("/main/app-label-fragment.fxml")
    private val appLabel: Label by fxid()
    private val leaveButton: JFXRippler by fxid()

    companion object {
        const val DEF_TITLE = "Music Streamer"
    }

    @PostConstruct
    private fun onInit() {
        clean()
    }

    fun setRoom(roomName: String) {
        appLabel.text = "Room: $roomName"
        leaveButton.show()
    }

    fun clean() {
        appLabel.text = DEF_TITLE
        leaveButton.hide()
    }
}