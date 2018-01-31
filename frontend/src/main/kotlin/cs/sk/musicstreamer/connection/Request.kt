package cs.sk.musicstreamer.connection


sealed class Request(private val method: String)
class AuthRequest(private val name: String) : Request("INIT")
class JoinRequest(private val roomName: String) : Request("JOIN")
class RoomsRequest(private val rooms: String) : Request("ROOMS")
class LeaveRoomRequest : Request("LEAVE")
class UploadRequest : Request("UPLOAD")

