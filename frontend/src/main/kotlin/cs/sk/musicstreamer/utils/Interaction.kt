package cs.sk.musicstreamer.utils

import com.jfoenix.controls.JFXListView
import java.util.concurrent.atomic.AtomicBoolean


class Informer(private val wasInformed: AtomicBoolean = AtomicBoolean(false)) {
    fun ifNotInformed(f: () -> Unit) {
        if (!wasInformed.getAndSet(true)) {
            f()
        }
    }
}

fun JFXListView<*>.clearSelection() = selectionModel.select(-1)