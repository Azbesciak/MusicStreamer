package cs.sk.musicstreamer

import com.natpryce.konfig.*
import com.natpryce.konfig.ConfigurationProperties.Companion.fromResource
import com.natpryce.konfig.ConfigurationProperties.Companion.systemProperties
import javafx.application.Application
import javafx.application.Application.launch
import javafx.scene.Scene
import javafx.scene.layout.Pane
import javafx.stage.Stage

class Main : Application() {

    object server : PropertyGroup() {
        val host by intType
        val port by stringType
    }

    // github -> npryce/konfig
    //           sam016/J-Mic-Stream-Over-Socket
    override fun start(stage: Stage) {
        val config = systemProperties() overriding
                EnvironmentVariables() overriding
                fromResource("application.properties")
        val pane = Pane()
        val scene = Scene(pane, 800.0, 600.0)
        stage.scene = scene
        stage.show()
    }

}

fun main(args: Array<String>) {
    launch(Main::class.java, *args)
}