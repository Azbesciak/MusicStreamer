package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXRippler
import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.*
import io.datafx.controller.ViewController
import javafx.fxml.Initializable
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.delay
import kotlinx.coroutines.experimental.javafx.JavaFx
import mu.KLogging
import tornadofx.*
import java.net.URL
import java.util.*

// cannot be a @Controller because is created statically...
@ViewController("/main/main_view.fxml")
class MainView : View(), Initializable {

    override val root: StackPane by fxml("/main/main_view.fxml")
    private val communicationServerConnector: ReadWriteConnector by di()
    private val broadCastServer: ReadingConnector by di()
    private val authService: AuthService by di()

    private val connectButton: JFXRippler by fxid()

    companion object : KLogging()

    private val snackBar: JFXSnackbar by lazy { JFXSnackbar(root).apply { prefWidth = 300.0 } }


    override fun initialize(location: URL?, resources: ResourceBundle?) {
        kotlinx.coroutines.experimental.launch {
            connectButton.setOnMouseClicked {
                connect()
            }
            broadCastServer.addConnectionListener(ConnectionListener(
                    onConnection = ::sendBroadCastSubscription,
                    onError = { showSnackBar("An error occurred on broadcast channel") }
            ))

            communicationServerConnector.addConnectionListener(ConnectionListener(
                    onConnection = {
                        kotlinx.coroutines.experimental.launch(JavaFx) {
                            tryToAuthenticate()
                            broadCastServer.connect()
                        }
                    },
                    onError = {
                        kotlinx.coroutines.experimental.launch {
                            delay(5000)
                            connectButton.isDisable = false
                        }
                        broadCastServer.disconnect()
                        showSnackBar("Could not connect with server")
                    }
            ))
            connect()
        }
    }

    private fun showSnackBar(message: String) = snackBar.fireEvent(JFXSnackbar.SnackbarEvent(message))

    private fun sendBroadCastSubscription() {
        val userName = authService.getUserName()!!
        broadCastServer.send(
                request = SubscribeRequest(userName),
                onResponse = { logger.info { "BroadCast subscribed for $userName" } },
                onError = { e -> logger.error { "could not subscribe for Broadcast: ${e.body}" } }
        )
    }

    private fun connect() {
        connectButton.isDisable = true
        communicationServerConnector.connect()
    }

    private suspend fun tryToAuthenticate(): String {
        logger.info { "Requested authorization..." }
        val username = authService.requestAuthorization(root)
        logger.info { "Username: $username" }
        return username

    }
}