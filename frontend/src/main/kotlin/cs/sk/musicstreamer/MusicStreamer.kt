package cs.sk.musicstreamer

import cs.sk.musicstreamer.homepage.HomePageView
import org.springframework.boot.autoconfigure.SpringBootApplication

import de.felixroske.jfxsupport.AbstractJavaFxApplicationSupport
import tornadofx.*

@SpringBootApplication
class MusicStreamer(val serverConnector: ServerConnector) : App(HomePageView::class) {

    // github -> npryce/konfig
    //           sam016/J-Mic-Stream-Over-Socket
    //  https://www.felixroske.de/page/programmierung/index.html

}

fun main(args: Array<String>) {
    AbstractJavaFxApplicationSupport.launch(MusicStreamer::class.java, *args)
}