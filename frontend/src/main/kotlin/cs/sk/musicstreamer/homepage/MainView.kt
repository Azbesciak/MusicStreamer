package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXDrawer
import com.jfoenix.controls.JFXHamburger
import com.jfoenix.controls.JFXRippler
import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.*
import cs.sk.musicstreamer.connection.connectors.MainConnector
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.ConnectionListener
import cs.sk.musicstreamer.connection.connectors.ResponseListener
import cs.sk.musicstreamer.room.RoomView
import io.datafx.controller.flow.context.ViewFlowContext
import javafx.fxml.Initializable
import javafx.scene.control.Label
import javafx.scene.layout.Pane
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.delay
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import tornadofx.*
import java.net.URL
import java.util.*

// cannot be a @Controller because is created statically...
class MainView : View(), Initializable {

    private val viewContext: ViewFlowContext by di()

    override val root: StackPane by fxml("/main/main_view.fxml")

    private val roomsPane: StackPane by fxid()
    private val content: Pane by fxid()
    private val labelContainer: Pane by fxid()

    private val roomsBurger: JFXHamburger by fxid()
    private val drawer: JFXDrawer by fxid()
    private val connectButton: JFXRippler by fxid()
    private val snackBar: JFXSnackbar by lazy { JFXSnackbar(root).apply { prefWidth = 300.0 } }
    private val appLabel: AppLabel by di()


    private val roomsView: RoomsView by di()
    private val roomView: RoomView by di()

    private val communicationServerConnector: MainConnector by di()
    private val broadCastServer: BroadCastConnector by di()
    private val authService: AuthService by di()

    companion object : KLogging()

    override fun initialize(location: URL?, resources: ResourceBundle?) {
        launch(JavaFx) {
            viewContext.register(snackBar)
            initView()
            launch {
                initConnectors()
            }
        }
    }

    private fun initView() {
        content += roomView.apply {
            root.toBack()
        }
        labelContainer += appLabel
        with(drawer) {
            setOnDrawerOpening { onDrawerChange(true) }
            setOnDrawerClosing { onDrawerChange(false) }
            roomsPane.setOnMouseClicked({
                when {
                    isHidden || isHiding -> open()
                    else -> close()
                }
            })
            sidePane += roomsView.root
            toFront()
        }
    }

    private fun onDrawerChange(open: Boolean) =
            with(roomsBurger.animation) {
                rate = if (open) 1.0 else -1.0
                play()
            }

    private fun initConnectors() {
        connectButton.setOnMouseClicked {
            connect()
        }
        broadCastServer.addConnectionListener(ConnectionListener(
                onConnection = ::sendBroadCastSubscription,
                onError = { launch(JavaFx) { showSnackBar("An error occurred on broadcast channel") } }
        ))

        communicationServerConnector.addConnectionListener(ConnectionListener(
                onConnection = {
                    launch(JavaFx) {
                        tryToAuthenticate()
                        broadCastServer.connect()
                    }
                },
                onDisconnect = { launch(JavaFx) { onDisconnect() } },
                onError = {
                    launch(JavaFx) {
                        showSnackBar("Could not connect with server")
                        onDisconnect()
                    }
                }
        ))
        connect()
    }

    private suspend fun onDisconnect() {
        broadCastServer.disconnect()
        authService.cleanUser()
        delay(5000)
        connectButton.isDisable = false
        roomsView.clean()
        appLabel.clean()
    }

    private fun showSnackBar(message: String) = snackBar.fireEvent(JFXSnackbar.SnackbarEvent(message))

    private fun sendBroadCastSubscription() {
        val userName = authService.getUserName()!!
        broadCastServer.send(
                request = SubscribeRequest(userName),
                onResponse = {
                    logger.info { "BroadCast subscribed for $userName" }
                    drawer.open()
                },
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