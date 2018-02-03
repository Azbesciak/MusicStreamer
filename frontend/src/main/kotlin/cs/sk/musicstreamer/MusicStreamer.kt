package cs.sk.musicstreamer

import com.jfoenix.controls.JFXDecorator
import com.jfoenix.svg.SVGGlyph
import com.jfoenix.svg.SVGGlyphLoader
import cs.sk.musicstreamer.homepage.MainView
import org.springframework.boot.autoconfigure.SpringBootApplication

import javafx.application.Application
import javafx.scene.Scene
import javafx.scene.image.Image
import javafx.scene.shape.SVGPath
import javafx.stage.Screen
import javafx.stage.Stage
import org.springframework.boot.SpringApplication
import org.springframework.context.ConfigurableApplicationContext
import tornadofx.*
import java.io.BufferedReader
import java.io.InputStreamReader
import java.util.stream.Collectors
import kotlin.reflect.KClass

@SpringBootApplication
class MusicStreamer : App(MainView::class) {

    // github -> npryce/konfig
    //           sam016/J-Mic-Stream-Over-Socket
    //  https://www.felixroske.de/page/programmierung/index.html
    private lateinit var context: ConfigurableApplicationContext


    override fun init() {
        context = SpringApplication.run(this.javaClass)
        context.autowireCapableBeanFactory.autowireBean(this)
        FX.dicontainer = object : DIContainer {
            override fun <T : Any> getInstance(type: KClass<T>): T = context.getBean(type.java)
        }
    }

    override fun start(stage: Stage) {
        val decorator = JFXDecorator(stage, find(primaryView, scope).root).apply {
            isCustomMaximize = true
            text = "Music Streamer"
        }

        var width = 900.0
        var height = 700.0
        try {
            val bounds = Screen.getScreens()[0].bounds
            width = bounds.width / 2.5
            height = bounds.height / 1.35
        } catch (e: Exception) {}

        val decorated = Scene(decorator, width, height).apply {
            stylesheets += getCss("/css/style.css")
        }
        stage.apply {
            icons += Image("./icon.png")
            scene = decorated
            show()
        }
    }

    private fun getCss(location: String) = javaClass.getResource(location).toExternalForm()

    override fun stop() {
        super.stop()
        context.close()
    }

    companion object {
        @JvmStatic
        fun main(args: Array<String>) {
            Application.launch(MusicStreamer::class.java, *args)
        }
    }

}