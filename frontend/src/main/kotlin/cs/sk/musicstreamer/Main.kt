package cs.sk.musicstreamer

import javafx.application.Application
import javafx.application.Application.launch
import javafx.scene.Scene
import javafx.scene.layout.Pane
import javafx.stage.Stage

class Main: Application() {
    override fun start(stage: Stage) {
        val pane = Pane()
        val scene = Scene(pane, 800.0, 600.0)
        stage.scene = scene
        stage.show()
    }

}

fun main(args: Array<String>) {
    launch(Main::class.java, *args)
}