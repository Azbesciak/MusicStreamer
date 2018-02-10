package cs.sk.musicstreamer

import cs.sk.musicstreamer.connection.CommunicationServerConnector
import cs.sk.musicstreamer.connection.ReadWriteConnector
import org.springframework.boot.CommandLineRunner
import org.springframework.context.annotation.Configuration

@Configuration
class Initializer(
        private val communicationServerConnector: ReadWriteConnector
) : CommandLineRunner {
    override fun run(vararg args: String?) {
        communicationServerConnector.connect()
    }

}