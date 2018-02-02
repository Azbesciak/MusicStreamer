package cs.sk.musicstreamer.connection


sealed class Response<T>(val status: Int, val body: T)
class StringResponse(status: Int = 200, body: String = "OK"): Response<String>(status, body)
class ErrorResponse(status: Int = 500, body: String): Response<String>(status, body)