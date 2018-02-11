package cs.sk.musicstreamer.connection.socket

import java.util.concurrent.atomic.AtomicBoolean

interface SocketConsumer {
    fun setSocket(socket: SocketProxy)
    fun setRunningFlag(isRunning: AtomicBoolean)
    fun start()
}