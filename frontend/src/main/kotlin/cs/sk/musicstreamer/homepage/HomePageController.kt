package cs.sk.musicstreamer.homepage

import de.felixroske.jfxsupport.FXMLController
import javafx.event.Event
import javafx.fxml.FXML
import javafx.scene.control.Label
import javafx.scene.control.TextField

@FXMLController
class HomePageController {

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