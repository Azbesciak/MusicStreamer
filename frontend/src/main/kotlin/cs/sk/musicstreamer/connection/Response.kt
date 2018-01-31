package cs.sk.musicstreamer.connection


sealed class Response<T>(private val status: Int, private val body: T)
class StringResponse(status: Int = 200, body: String = "OK"): Response<String>(status, body)
class ErrorResponse<T>(status: Int = 500, body: Error<T>): Response<Error<T>>(status, body)

class Error<T>(val error: T)