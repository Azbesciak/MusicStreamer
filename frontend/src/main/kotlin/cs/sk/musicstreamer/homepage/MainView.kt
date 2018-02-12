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
import javafx.beans.binding.Bindings
import javafx.beans.binding.BooleanBinding
import javafx.beans.binding.BooleanExpression
import javafx.beans.property.BooleanProperty
import javafx.beans.property.SimpleBooleanProperty
import javafx.beans.value.ObservableBooleanValue
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

    override val root: StackPane by fxml("/main/main_view.fxml")

    private val infoService: InfoService by di()
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

    private var isConnected = SimpleBooleanProperty(false)

    companion object : KLogging()

    override fun initialize(location: URL?, resources: ResourceBundle?) {
        launch(JavaFx) {
            infoService.setSnackBar(snackBar)
            initView()
            launch {
                initConnectors()
            }
        }
    }

    private fun initView() {
        content += roomView
        labelContainer += appLabel
        onConnectionChange(false)
        with(drawer) {
            setOnDrawerOpening { onDrawerChange(true) }
            setOnDrawerClosing { onDrawerChange(false) }
            roomsPane.setOnMouseClicked({
                when {
                    isHidden || isHiding -> openAtFront()
                    else -> closeToBack()
                }
            })
            isConnected.addListener {_, _, connected -> onConnectionChange(connected) }
            roomsView.addJoinListener { closeToBack() }
            sidePane += roomsView.root
        }
    }

    private fun onConnectionChange(connected: Boolean) {
        roomsPane.isDisable = connected.not()
        with (roomView.root) {
            isDisable = connected.not()
        }
    }

    private fun onDrawerChange(open: Boolean) =
            with(roomsBurger.animation) {
                rate = if (open) 1.0 else -1.0
                play()
            }

    private fun initConnectors() {

        connectButton.setOnMouseClicked { connect() }
        broadCastServer.addConnectionListener(ConnectionListener(
                onConnection = ::sendBroadCastSubscription,
                onError = { launch(JavaFx) { infoService.showSnackBar("An error occurred on broadcast channel") } }
        ))

        communicationServerConnector.addConnectionListener(ConnectionListener(
                onConnection = {
                    launch(JavaFx) {
                        tryToAuthenticate()
                        launch { broadCastServer.connect() }
                        isConnected.value = true
                    }
                },
                onDisconnect = { launch(JavaFx) { onDisconnect() } },
                onError = {
                    launch(JavaFx) {
                        infoService.showSnackBar("Could not connect with server")
                        onDisconnect()
                    }
                }
        ))
        connect()
    }

    private suspend fun onDisconnect() {
        drawer.closeToBack()
        isConnected.value = false
        broadCastServer.disconnect()
        authService.cleanUser()
        roomsView.clean()
        roomView.clean()
        delay(5000)
        connectButton.isDisable = false
    }



    private fun sendBroadCastSubscription() {
        val userName = authService.getUserName()!!
        broadCastServer.send(
                request = SubscribeRequest(userName),
                onResponse = {
                    logger.info { "BroadCast subscribed for $userName" }
                    launch(JavaFx) { drawer.openAtFront() }
                },
                onError = { e -> logger.error { "could not subscribe for Broadcast: ${e.body}" } }
        )
    }

    private fun JFXDrawer.openAtFront() = open().also { toFront() }
    private fun JFXDrawer.closeToBack() = close().also { toBack() }

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