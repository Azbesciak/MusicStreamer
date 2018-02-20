package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXDrawer
import com.jfoenix.controls.JFXHamburger
import com.jfoenix.controls.JFXRippler
import com.jfoenix.controls.JFXSnackbar
import cs.sk.musicstreamer.authorization.AuthService
import cs.sk.musicstreamer.connection.StreamSubscribeRequest
import cs.sk.musicstreamer.connection.SubscribeRequest
import cs.sk.musicstreamer.connection.connectors.BroadCastConnector
import cs.sk.musicstreamer.connection.connectors.ConnectionListener
import cs.sk.musicstreamer.connection.connectors.MainConnector
import cs.sk.musicstreamer.connection.connectors.StreamingConnector
import cs.sk.musicstreamer.room.RoomView
import javafx.beans.property.SimpleBooleanProperty
import javafx.fxml.Initializable
import javafx.scene.layout.Pane
import javafx.scene.layout.StackPane
import kotlinx.coroutines.experimental.delay
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import tornadofx.*
import java.net.URL
import java.util.*
import kotlin.coroutines.experimental.suspendCoroutine

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
    private val streamingConnector: StreamingConnector by di()
    private val authService: AuthService by di()

    private var isConnected = SimpleBooleanProperty(false)

    companion object : KLogging() {
        const val MAX_STREAMER_ATTEMPTS = 10
    }

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
            isConnected.addListener { _, _, connected -> onConnectionChange(connected) }
            roomsView.addJoinListener { closeToBack() }
            sidePane += roomsView.root
        }

        appLabel.addLeaveListener {
            launch(JavaFx) {
                appLabel.clean()
                roomView.clean()
                roomsView.leaveRoom()
                drawer.openAtFront()
                infoService.showSnackBar("You have left the room")
            }
        }
    }

    private fun onConnectionChange(connected: Boolean) {
        roomsPane.isDisable = connected.not()
        with(roomView.root) {
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
        broadCastServer.addConnectionListener(manageStreamerLifecycle())
        broadCastServer.addMessagesListener(streamingConnector.getFrameListener())
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

    private fun manageStreamerLifecycle(): ConnectionListener {
        return ConnectionListener(
                onConnection = {
                    launch {
                        var attemption = 0
                        while (attemption++ < MAX_STREAMER_ATTEMPTS) {
                            val port = streamingConnector.connect()
                            if (port != null) {
                                val connected = suspendCoroutine<Boolean> { cont ->
                                    communicationServerConnector.send(
                                            StreamSubscribeRequest(port),
                                            onResponse = { cont.resume(true) },
                                            onError = { cont.resume(false) })

                                }
                                if (connected) {
                                    logger.info { "subscribed on stream" }
                                    return@launch
                                } else {
                                    streamingConnector.close()
                                }
                                delay(500)
                            } else {
                                logger.info { "could not assign port for streamer...$attemption" }
                            }
                        }
                        launch(JavaFx) { infoService.showSnackBar("Could not subscribe on streaming") }
                    }
                },
                onError = {
                    streamingConnector.close()
                },
                onDisconnect = {
                    streamingConnector.close()
                }
        )
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
                onError = { logger.error { "could not subscribe for Broadcast: ${it.body}" } }
        )
    }

    private fun JFXDrawer.openAtFront() = open().also { toFront() }
    private fun JFXDrawer.closeToBack() = close().also { launch(JavaFx) { delay(1000);toBack() } }

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