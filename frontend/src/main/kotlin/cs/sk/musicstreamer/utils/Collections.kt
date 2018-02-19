package cs.sk.musicstreamer.utils

import com.fasterxml.jackson.databind.JsonNode
import java.util.concurrent.ConcurrentLinkedQueue

inline fun <T> ConcurrentLinkedQueue<T>.whileNotEmpty(f: (T) -> Unit) {
    while (isNotEmpty()) {
        f(poll())
    }
}


fun JsonNode.getStrings(field: String) = get(field).map {it.asText()}