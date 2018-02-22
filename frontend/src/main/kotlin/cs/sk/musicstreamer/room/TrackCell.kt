package cs.sk.musicstreamer.room

import com.jfoenix.controls.JFXListCell
import javafx.event.EventHandler
import javafx.scene.input.ClipboardContent
import javafx.scene.input.DragEvent
import javafx.scene.input.TransferMode

class TrackCell(
        private val onSwap: (from: Int, to: Int) -> Unit
) : JFXListCell<String>() {
    init {
        setOnDragDetected { event ->
            if (item == null) {
                return@setOnDragDetected
            }

            val dragboard = startDragAndDrop(TransferMode.MOVE)
            val content = ClipboardContent()
            content.putString(item)
            content.putUrl(index.toString())
            dragboard.setContent(content)

            event.consume()
        }

        setOnDragOver { event ->
            if (event.gestureSource !== this && event.dragboard.hasString()) {
                event.acceptTransferModes(TransferMode.MOVE)
            }

            event.consume()
        }

        setOnDragEntered { event ->
            if (event.gestureSource !== this && event.dragboard.hasString()) {
                opacity = 0.3
            }
        }

        setOnDragExited { event ->
            if (event.gestureSource !== this && event.dragboard.hasString()) {
                opacity = 1.0
            }
        }

        setOnDragDropped { event ->
            if (item == null) {
                return@setOnDragDropped
            }

            val db = event.dragboard
            var success = false

            if (db.hasString() and db.hasUrl()) {
                val draggedIdx = db.url.toInt()
                val thisIdx = index
                onSwap(draggedIdx, thisIdx)
                success = true
            }
            event.isDropCompleted = success

            event.consume()
        }

        onDragDone = EventHandler<DragEvent> { it.consume() }
    }

    override fun updateItem(item: String?, empty: Boolean) {
        super.updateItem(item, empty)
        isFocused = false
    }
}