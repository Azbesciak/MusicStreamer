package cs.sk.musicstreamer.homepage

import cs.sk.musicstreamer.ServerConnector
import javafx.event.Event
import javafx.fxml.FXML
import javafx.scene.control.Label
import javafx.scene.control.TextField
import org.springframework.stereotype.Controller

@Controller
class MainViewController(
        val serverConnector: ServerConnector) {

    @FXML
    private val helloLabel: Label? = null

    @FXML
    private val nameField: TextField? = null


    @FXML
    private fun setHelloText(event: Event) {
        //       final String textToBeShown = actionService.processName(nameField.getText());
        helloLabel!!.text = "alamakota"
    }
}