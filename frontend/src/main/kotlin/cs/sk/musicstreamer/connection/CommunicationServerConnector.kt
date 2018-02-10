package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.JsonNode
import cs.sk.musicstreamer.utils.whileNotEmpty
import org.springframework.beans.factory.annotation.Value
import org.springframework.stereotype.Service
import java.nio.channels.AsynchronousSocketChannel
import java.util.concurrent.ConcurrentLinkedQueue


@Service
class CommunicationServerConnector(
        @Value("\${server.host}") host: String,
        @Value("\${server.connection.port}") port: Int
) : ServerConnector(host, port, "Communication server") {
    override fun parseResponse(status: Int, body: JsonNode): Response<*> {
        return defaultResponse(status, body)
    }

    private val queue = ConcurrentLinkedQueue<ConnectionRequest>()

    override suspend fun onConnection(socket: AsynchronousSocketChannel) =
            queue.whileNotEmpty {
                val wrote = socket.write(it.request)
                if (wrote <= 0) {
                    logger.error { "Could not write to $serverDescription" }
                    it.onError(ErrorResponse(body = "Couldn't send request to server"))
                } else {
                    socket.read(it.onResponse, it.onError)
                }
            }

    fun send(request: Request, onResponse: (Response<*>) -> Unit, onError: (ErrorResponse) -> Unit) =
            queue.add(ConnectionRequest(request, onResponse, onError))
}


