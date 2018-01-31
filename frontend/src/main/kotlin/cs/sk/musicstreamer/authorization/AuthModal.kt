package cs.sk.musicstreamer.authorization

import cs.sk.musicstreamer.connection.ServerConnector
import javafx.scene.Parent
import javafx.scene.control.TextField
import javafx.stage.Modality
import org.springframework.stereotype.Controller
import tornadofx.*

@Controller
class AuthModal(
        private val serverConnector: ServerConnector
): View() {
    override val root: Parent by fxml("/auth/auth_view.fxml")
    private val username: TextField by fxid()

    suspend fun showLoginModal() {
        openModal(modality = Modality.NONE)
    }
}