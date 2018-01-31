package cs.sk.musicstreamer.homepage

import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.ServerConnector
import javafx.fxml.Initializable
import javafx.scene.layout.AnchorPane
import kotlinx.coroutines.experimental.javafx.JavaFx
import mu.KLogging
import org.springframework.stereotype.Controller
import tornadofx.*
import java.net.URL
import java.util.*

@Controller
class MainView : View(), Initializable {
    override val root: AnchorPane by fxml("/main/main_view.fxml")
    private val serverConnector: ServerConnector by di()
    private val authService: AuthService by di()

    companion object : KLogging()

    override fun initialize(location: URL?, resources: ResourceBundle?) {
        logger.info { "Requested authorization..." }
        kotlinx.coroutines.experimental.launch(JavaFx) {
            authService.requestAuthorization()
        }
    }
}