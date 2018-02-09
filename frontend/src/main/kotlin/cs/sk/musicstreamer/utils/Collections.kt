package cs.sk.musicstreamer.utils

import java.util.concurrent.ConcurrentLinkedQueue

inline fun <T> ConcurrentLinkedQueue<T>.whileNotEmpty(f: (T) -> Unit) {
    while (isNotEmpty()) {
        f(poll())
    }
}