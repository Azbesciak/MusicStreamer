package cs.sk.musicstreamer.connection.connectors

import cs.sk.musicstreamer.connection.connectors.ConnectionListener
import cs.sk.musicstreamer.connection.socket.SocketProxy
import cs.sk.musicstreamer.connection.socket.SocketConsumer
import mu.KLogging
import java.util.concurrent.CopyOnWriteArrayList
import java.util.concurrent.atomic.AtomicBoolean

class ConnectorSlave(host: String, port: Int,
                     private val consumers: Array<SocketConsumer>,
                     private val listeners: CopyOnWriteArrayList<ConnectionListener>) {

    companion object : KLogging()

    private val isConnected = AtomicBoolean(false)
    private val sock = SocketProxy(host, port, isConnected)
    val description = sock.socketDescription

    @Synchronized
    suspend fun connect() {
        if (!isConnected.get()) {
            try {
                sock.connect()
                consumers.forEach {
                    it.setSocket(sock)
                    it.setRunningFlag(isConnected)
                    it.start()
                }
                listeners.forEach { it.onConnection() }
            } catch (e: Throwable) {
                onError(e)
            }
        }
    }

    private fun onError(e: Throwable) {
        listeners.forEach { it.onError(Error(e)) }
    }

    @Synchronized
    fun disconnect() = sock.close()

    fun isConnected() = isConnected.get()
}