package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.*
import cs.sk.musicstreamer.connection.listeners.IdentifiedMessageListener
import cs.sk.musicstreamer.connection.listeners.UndefinedMessageListener
import cs.sk.musicstreamer.connection.socket.SocketReader
import cs.sk.musicstreamer.connection.socket.SocketWriter
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import java.util.concurrent.CopyOnWriteArrayList

abstract class Connector(host: String, port: Int) {
    companion object : KLogging()

    protected val connectionListeners = CopyOnWriteArrayList(initialConnectionListeners())
    protected val writer = SocketWriter()
    protected val messagesListener = UndefinedMessageListener(initialMessagesListeners())
    protected val responseListener = IdentifiedMessageListener()
    protected val reader = SocketReader(listOf(messagesListener, responseListener), ::onError)
    protected val con = ConnectorSlave(host, port, arrayOf(writer, reader), connectionListeners)

    fun connect() = launch {
        con.connect()
    }

    fun disconnect() {
        if (con.disconnect()) {
            connectionListeners.forEach { it.onDisconnect() }
        }
    }

    fun isConnected() = con.isConnected()
    fun addConnectionListener(listener: ConnectionListener) = connectionListeners.add(listener)

    protected open fun initialConnectionListeners() = listOf<ConnectionListener>()
    protected open fun initialMessagesListeners() = listOf<ResponseListener>()

    fun send(request: Request, onResponse: (JsonResponse) -> Unit, onError: (ErrorResponse) -> Unit) {
        launch {
            try {
                responseListener.waitForResponse(request, ResponseListener(onError = onError, onResponse = onResponse))
                writer.write(request)
            } catch (e: Error) {
                responseListener.removeListener(request)
                onError(ErrorResponse(500, "Error while managing request from ${this::class.simpleName} - ${con.description}"))
                if (e is ConnectionError) {
                    onError(e)
                }
            }
        }
    }

    private fun onError(e: Throwable) {
        logger.error { "Error occured: ${e.message}" }
        connectionListeners.forEach { it.onError(e) }
    }
}