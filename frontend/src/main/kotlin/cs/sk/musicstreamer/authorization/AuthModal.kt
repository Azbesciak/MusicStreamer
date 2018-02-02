package cs.sk.musicstreamer.authorization

import com.jfoenix.controls.JFXButton
import cs.sk.musicstreamer.connection.ServerConnector
import tornadofx.*
import com.jfoenix.controls.JFXDialog
import com.jfoenix.controls.JFXTextField
import com.jfoenix.validation.base.ValidatorBase
import cs.sk.musicstreamer.connection.AuthRequest
import javafx.event.EventHandler
import javafx.scene.control.TextInputControl
import mu.KLogging
import javafx.scene.layout.StackPane
import org.springframework.stereotype.Component
import javax.annotation.PostConstruct


@Component
class AuthModal(
        private val serverConnector: ServerConnector
): View() {

    companion object : KLogging()

    override val root: StackPane by fxml("/auth/auth_view.fxml")
    private val username: JFXTextField by fxid()
    private val dialog: JFXDialog by fxid()
    private val acceptButton: JFXButton by fxid()

    suspend fun showLoginModal(container: StackPane) {
        dialog.overlayCloseProperty().set(false)
        dialog.show(container)
    }

    @PostConstruct
    fun assignListeners() {
        acceptButton.isDisable = true
        username.textProperty().addListener { _ ->
            acceptButton.isDisable = username.validate().not()
        }

        val invalidUserName = InvalidUserName()
        username.validators.add(invalidUserName)
        acceptButton.onMouseClicked = EventHandler {
            val userNameValue = username.text
            serverConnector.send(AuthRequest(userNameValue), {}, {
                invalidUserName addToBlackList userNameValue
                username.validate()
            })
        }
    }

    class InvalidUserName: ValidatorBase() {
        private val invalidUserNames = mutableSetOf<String>()

        infix fun addToBlackList(userName: String) = invalidUserNames.add(userName)

        override fun eval() {
            val text = (srcControl.value as TextInputControl).textProperty().get()
            hasErrors.set(invalidUserNames.contains(text))
        }

        override fun getMessage(): String {
            return "That name is already used"
        }
    }

    class InternalServerError: ValidatorBase() {
        override fun eval() {
        }

    }
}