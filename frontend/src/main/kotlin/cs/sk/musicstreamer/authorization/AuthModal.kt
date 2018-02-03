package cs.sk.musicstreamer.authorization

import com.jfoenix.controls.JFXButton
import cs.sk.musicstreamer.connection.ServerConnector
import tornadofx.*
import com.jfoenix.controls.JFXDialog
import com.jfoenix.controls.JFXTextField
import cs.sk.musicstreamer.connection.AuthRequest
import cs.sk.musicstreamer.utils.InvalidFieldValueValidator
import cs.sk.musicstreamer.utils.VetoValidator
import javafx.event.EventHandler
import mu.KLogging
import javafx.scene.layout.StackPane
import org.springframework.stereotype.Component
import javax.annotation.PostConstruct


@Component
class AuthModal(
        private val serverConnector: ServerConnector
) : View() {

    companion object : KLogging()

    override val root: StackPane by fxml("/auth/auth_view.fxml")
    private val username: JFXTextField by fxid()
    private val dialog: JFXDialog by fxid()
    private val acceptButton: JFXButton by fxid()

    private lateinit var userName: String


    fun showLoginModal(container: StackPane, onSuccess: (String) -> Unit) {
        dialog.show(container)
        dialog.onDialogClosed = EventHandler { onSuccess(userName) }
    }

    @PostConstruct
    fun assignListeners() {
        acceptButton.isDisable = true
        username.textProperty().addListener { _ ->
            acceptButton.isDisable = username.validate().not()
        }

        val invalidUserName = InvalidFieldValueValidator("That name can't be used")
        val internalServerError = VetoValidator()
        username.validators.addAll(invalidUserName, internalServerError)
        acceptButton.onMouseClicked = EventHandler {
            val userNameValue = username.text
            serverConnector.send(AuthRequest(userNameValue), {
                userName = userNameValue
                dialog.close()
            }, {
                when (it.status) {
                    500 -> internalServerError.setInvalid()
                    else -> invalidUserName addToBlackList userNameValue
                }
                username.validate()
            })
        }
        dialog.overlayCloseProperty().set(false)
    }
}