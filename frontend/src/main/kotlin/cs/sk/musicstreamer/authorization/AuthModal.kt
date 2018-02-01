package cs.sk.musicstreamer.authorization

import com.jfoenix.controls.JFXButton
import cs.sk.musicstreamer.connection.ServerConnector
import tornadofx.*
import com.jfoenix.controls.JFXDialog
import com.jfoenix.controls.JFXTextField
import javafx.fxml.Initializable
import mu.KLogging
import javafx.scene.layout.StackPane
import org.springframework.stereotype.Component
import java.net.URL
import java.util.*


@Component
class AuthModal(
        private val serverConnector: ServerConnector
): View(), Initializable {
    override fun initialize(location: URL?, resources: ResourceBundle?) {
        logger.info { "Initialized..." }
    }

    companion object : KLogging()

    override val root: StackPane by fxml("/auth/auth_view.fxml")
    private val username: JFXTextField by fxid()
    private val dialog: JFXDialog by fxid()
    private val acceptButton: JFXButton by fxid()

    suspend fun showLoginModal(container: StackPane) {
        dialog.overlayCloseProperty().set(false)
        dialog.show(container)
    }
}