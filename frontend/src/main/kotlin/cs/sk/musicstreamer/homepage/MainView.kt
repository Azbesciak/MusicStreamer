package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.ServerConnector
import io.datafx.controller.ViewController
import javafx.fxml.Initializable
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.javafx.JavaFx
import mu.KLogging
import tornadofx.*
import java.net.URL
import java.util.*

// cannot be a @Controller because is created statically...
@ViewController("/main/main_view.fxml")
class MainView : View(), Initializable {

    override val root: StackPane by fxml("/main/main_view.fxml")
    private val serverConnector: ServerConnector by di()
    private val authService: AuthService by di()

    companion object : KLogging()

    private val snackBar: JFXSnackbar by lazy { JFXSnackbar(root).apply { prefWidth = 300.0 } }


    override fun initialize(location: URL?, resources: ResourceBundle?) {
        kotlinx.coroutines.experimental.launch(JavaFx) {
            if (serverConnector.isRunning()) {
                tryToAuthenticate()
            } else {
                snackBar.fireEvent(JFXSnackbar.SnackbarEvent("Could not connect with server"))
            }
        }
    }

    private suspend fun tryToAuthenticate() {
        logger.info { "Requested authorization..." }
        val username = authService.requestAuthorization(root)
        logger.info { "Username: $username" }
    }
}