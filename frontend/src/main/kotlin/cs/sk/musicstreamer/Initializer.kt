package cs.sk.musicstreamer

import cs.sk.musicstreamer.connection.connectors.MainConnector
import org.springframework.boot.CommandLineRunner
import org.springframework.context.annotation.Configuration

@Configuration
class Initializer(
        private val communicationServerConnector: MainConnector
) : CommandLineRunner {
    override fun run(vararg args: String?) {
//        communicationServerConnector.connect()
    }

}