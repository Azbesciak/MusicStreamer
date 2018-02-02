package cs.sk.musicstreamer.connection


sealed class Request(val method: String)
class AuthRequest(val name: String) : Request("INIT")
class JoinRequest(val roomName: String) : Request("JOIN")
class RoomsRequest(val rooms: String) : Request("ROOMS")
class LeaveRoomRequest : Request("LEAVE")
class UploadRequest : Request("UPLOAD")

