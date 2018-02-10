package cs.sk.musicstreamer.connection

import com.fasterxml.jackson.databind.JsonNode


sealed class Response<out T>(val status: Int, var id: String?, val body: T) {
    override fun toString()= "Response{status:$status, id:$id, body:$body}"
}
class StringResponse(status: Int = 200, body: String = "OK", id: String? = null) : Response<String>(status, id, body)
class JsonResponse(status: Int = 200, body: JsonNode, id: String? = null) : Response<JsonNode>(status, id, body)
class ErrorResponse(status: Int = 500, body: String, id: String? = null) : Response<String>(status, id, body)