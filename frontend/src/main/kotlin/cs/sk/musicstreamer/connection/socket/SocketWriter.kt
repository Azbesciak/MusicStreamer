package cs.sk.musicstreamer.connection.socket

import cs.sk.musicstreamer.connection.Request
import mu.KLogging
import java.util.concurrent.atomic.AtomicBoolean

class SocketWriter : SocketConsumer {
    private lateinit var socketProxy: SocketProxy
    private lateinit var isRunning: AtomicBoolean

    companion object : KLogging()

    override fun setSocket(socket: SocketProxy) {
        this.socketProxy = socket
    }

    override fun setRunningFlag(isRunning: AtomicBoolean) {
        this.isRunning = isRunning
    }

    override fun start() {
    }

    suspend fun write(request: Request): Int {
        logger.debug { "Writing to ${socketProxy.socketDescription}: $request" }
        return socketProxy.write(request)
    }
}