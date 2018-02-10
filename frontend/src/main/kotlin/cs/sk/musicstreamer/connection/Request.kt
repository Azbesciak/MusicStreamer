package cs.sk.musicstreamer.connection

import java.util.*


sealed class Request(val method: String, val id: String = UUID.randomUUID().toString())
class AuthRequest(val name: String) : Request("INIT")
class JoinRequest(val roomName: String) : Request("JOIN")
class RoomsRequest(val rooms: String) : Request("ROOMS")
class LeaveRoomRequest : Request("LEAVE")
class UploadRequest : Request("UPLOAD")
class SubscribeRequest(val name: String) : Request("SUBSCRIBE")

