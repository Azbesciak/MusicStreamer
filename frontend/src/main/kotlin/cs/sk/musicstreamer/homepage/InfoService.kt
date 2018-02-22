package cs.sk.musicstreamer.homepage

import com.jfoenix.controls.JFXSnackbar
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import org.springframework.stereotype.Service

@Service
class InfoService {

    private lateinit var snackBar: JFXSnackbar

    fun setSnackBar(snackBar: JFXSnackbar) {
        this.snackBar = snackBar
    }

    fun showSnackBar(message: String) = launch(JavaFx) {
        snackBar.fireEvent(JFXSnackbar.SnackbarEvent(message))
    }

    fun show(message: String) {
        launch(JavaFx) {showSnackBar(message)}
    }
}