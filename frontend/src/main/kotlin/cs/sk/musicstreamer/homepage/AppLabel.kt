package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXRippler
import cs.sk.musicstreamer.connection.LeaveRoomRequest
import cs.sk.musicstreamer.connection.connectors.MainConnector
import javafx.scene.control.Label
import javafx.scene.layout.HBox
import org.springframework.stereotype.Component
import tornadofx.*
import javax.annotation.PostConstruct

@Component
class AppLabel(
    private val mainConnector: MainConnector
): Fragment() {
    override val root: HBox by fxml("/main/app-label-fragment.fxml")
    private val appLabel: Label by fxid()
    private val leaveButton: JFXRippler by fxid()
    private val leaveListeners = mutableListOf<()->Unit>()

    companion object {
        const val DEF_TITLE = "Music Streamer"
    }

    @PostConstruct
    private fun onInit() {
        clean()
        initLeaveButton()
    }

    fun setRoom(roomName: String) {
        appLabel.text = "Room: $roomName"
        leaveButton.show()
    }

    fun clean() {
        appLabel.text = DEF_TITLE
        leaveButton.hide()
    }

    fun addLeaveListener(listener: () -> Unit)  = leaveListeners.add(listener)

    fun initLeaveButton() = leaveButton.setOnMouseClicked {
        mainConnector.send(LeaveRoomRequest(),
                onResponse = {
                    leaveListeners.forEach { it() }
                },
                onError = {
                    log.warning("could not leave room")
                })
    }
}