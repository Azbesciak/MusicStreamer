package cs.sk.musicstreamer

import mu.KLogging
import java.net.Socket
import javax.sound.sampled.AudioSystem
import javax.sound.sampled.SourceDataLine
import javax.sound.sampled.DataLine
import javax.sound.sampled.AudioFormat
import javax.sound.sampled.LineUnavailableException
import java.io.IOException
import java.io.InputStream


class Client @Throws(IOException::class, LineUnavailableException::class)
constructor(private val serverName: String = "127.0.0.1") {

    private lateinit var speaker: SourceDataLine
    private lateinit var streamIn: InputStream
    private lateinit var server: Socket
    private var running = true

    init {
        init()
    }

    @Throws(LineUnavailableException::class)
    private fun init() {
        //  specifying the audio format
        val format = AudioFormat(8000f, // Sample Rate
                16, // Size of SampleBits
                1, // Number of Channels
                true, // Is Signed?
                false   // Is Big Endian?
        )

        //  creating the DataLine Info for the speaker format
        val speakerInfo = DataLine.Info(SourceDataLine::class.java, format)

        //  getting the mixer for the speaker
        speaker = AudioSystem.getLine(speakerInfo) as SourceDataLine
        speaker.open(format)
    }

    fun start() {
        try {
            logger.info { "Connecting to server @$serverName:$PORT" }

            //  creating the socket and connect to the server
            server = Socket(serverName, PORT)
            logger.info { "Connected to: " + server.remoteSocketAddress }

            //  gettting the server stream
            streamIn = server.getInputStream()

            speaker.start()

            val data = ByteArray(8000)
            logger.info { "Waiting for data..." }
            while (running) {

                //  checking if the data is available to speak
                if (streamIn.available() <= 0)
                    continue   //  data not available so continue back to start of loop

                //  count of the data bytes read
                val readCount = streamIn.read(data, 0, data.size)

                if (readCount > 0) {
                    speaker.write(data, 0, readCount)
                }
            }
            //honestly.... the control never reaches here.
            speaker.drain()
            speaker.close()
        } catch (e: Exception) {
           logger.error { e }
        }

    }

    companion object : KLogging() {
        var PORT = 3000
    }
}