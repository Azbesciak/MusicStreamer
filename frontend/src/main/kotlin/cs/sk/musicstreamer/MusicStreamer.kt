package cs.sk.musicstreamer

import cs.sk.musicstreamer.homepage.MainView
import org.springframework.boot.autoconfigure.SpringBootApplication

import javafx.application.Application
import org.springframework.boot.SpringApplication
import org.springframework.context.ConfigurableApplicationContext
import tornadofx.*
import kotlin.reflect.KClass

@SpringBootApplication
//@ComponentScan(basePackages = ["cs.sk.musicstreamer"])
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