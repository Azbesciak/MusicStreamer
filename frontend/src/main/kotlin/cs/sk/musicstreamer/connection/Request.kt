package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.ObjectMapper
import java.util.*


sealed class Request(val method: String, val id: String = UUID.randomUUID().toString()) {
    companion object {
        val objMapper = ObjectMapper()
    }
    override fun toString() = objMapper.writeValueAsString(this)
}
class AuthRequest(val name: String) : Request("INIT")
class JoinRequest(val roomName: String) : Request("JOIN")
class RoomsRequest(val rooms: String) : Request("ROOMS")
class LeaveRoomRequest : Request("LEAVE")
class UploadRequest(val trackName: String, val trackFileSize: Long) : Request("UPLOAD")
class SubscribeRequest(val name: String) : Request("SUBSCRIBE")
class UploadTokenRequest(val name: String, val token: String) : Request("UPLOAD_TOKEN")
class StreamSubscribeRequest(val streamingPort: Int): Request("STREAM_SUBSCRIBE")
class QueueTrackRequest(val trackName: String): Request("QUEUE_TRACK")

