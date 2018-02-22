package cs.sk.musicstreamer.room

import com.jfoenix.controls.JFXButton
import cs.sk.musicstreamer.connection.connectors.StreamingListener
import cs.sk.musicstreamer.connection.connectors.Switchable
import cs.sk.musicstreamer.homepage.InfoService
import cs.sk.musicstreamer.utils.SVGFactory
import javafx.beans.property.SimpleBooleanProperty
import kotlinx.coroutines.experimental.javafx.JavaFx
import kotlinx.coroutines.experimental.launch
import mu.KLogging
import org.springframework.stereotype.Service
import tornadofx.*
import javax.sound.sampled.*


@Service
class MusicPlayer(
        private val infoService: InfoService
) {
    companion object: KLogging()

    private var audioLine: SourceDataLine? = null
    private lateinit var provider: Switchable
    private lateinit var room: RoomView
    private val isPlaying = SimpleBooleanProperty(false)

    fun registerControllers(player: JFXButton, skipper: JFXButton) {
        player.graphic = SVGFactory.play
        isPlaying.onChange {
            if (it) {
                player.graphic = SVGFactory.stop
                provider.start()
            } else {
                player.graphic = SVGFactory.play
                provider.stop()
            }
        }
        player.setOnMouseClicked {
            isPlaying.set(isPlaying.get().not())
        }
        skipper.graphic = SVGFactory.next
        skipper.setOnMouseClicked { room.requestNextTrack() }
    }

    fun setRoom(room: RoomView) {
        this.room = room
    }

    fun getStreamingListener() = StreamingListener(
            formatListener = { audioFormat -> prepareNewTrack(audioFormat) },
            onNewData = { audioData -> pushAudioData(audioData) },
            switch = { provider = it },
            onError = { launch(JavaFx) { infoService.showSnackBar(it.message?: "Unknown error with sound streaming") }}
    )


    @Synchronized
    fun prepareNewTrack(audioFormat: AudioFormat) {
        logger.debug("Received audio format of a new track\n$audioFormat")
        audioLine?.close()

        audioLine = AudioSystem.getLine(
                DataLine.Info(SourceDataLine::class.java, audioFormat)) as SourceDataLine
        try {
            audioLine!!.open(audioFormat)
            audioLine!!.start()
        } catch (t: Throwable) {
            infoService.showSnackBar("Error with current track: ${t.message}")
            audioLine?.close()
            audioLine = null
            isPlaying.set(false)
        }
    }

    @Synchronized
    fun pushAudioData(audioData: ByteArray) {
        audioLine?.write(audioData, 0, audioData.size)
    }

    fun clear() {
        audioLine?.close()
        isPlaying.set(false)
    }
}

//
//class VUServer {
//
//    internal var byteOutputStream: ByteArrayOutputStream? = null
//    internal var adFormat: AudioFormat? = null
//    internal var targetDataLine: TargetDataLine? = null
//    internal var InputStream: AudioInputStream
//    internal var sourceLine: SourceDataLine
//
//    private val audioFormat: AudioFormat
//        get() {
//            val sampleRate = 16000.0f
//            val sampleInbits = 16
//            val channels = 1
//            val signed = true
//            val bigEndian = false
//            return AudioFormat(sampleRate, sampleInbits, channels, signed, bigEndian)
//        }
//
//    fun runVOIP() {
//        try {
//            val serverSocket = DatagramSocket(9786)
//            val receiveData = ByteArray(10000)
//            while (true) {
//                val receivePacket = DatagramPacket(receiveData, receiveData.size)
//                serverSocket.receive(receivePacket)
//                println("RECEIVED: " + receivePacket.address.hostAddress + " " + receivePacket.port)
//                try {
//                    val audioData = receivePacket.data
//                    val byteInputStream = ByteArrayInputStream(audioData)
//                    val adFormat = audioFormat
//                    InputStream = AudioInputStream(byteInputStream, adFormat, (audioData.size / adFormat.frameSize).toLong())
//                    val dataLineInfo = DataLine.Info(SourceDataLine::class.java, adFormat)
//                    sourceLine = AudioSystem.getLine(dataLineInfo) as SourceDataLine
//                    sourceLine.open(adFormat)
//                    sourceLine.start()
//                    val playThread = Thread(PlayThread())
//                    playThread.start()
//                } catch (e: Exception) {
//                    println(e)
//                    System.exit(0)
//                }
//
//            }
//        } catch (e: Exception) {
//            e.printStackTrace()
//        }
//
//    }
//
//    internal inner class PlayThread : Thread() {
//
//        var tempBuffer = ByteArray(10000)
//
//        override fun run() {
//            try {
//                var cnt: Int
//                while ((cnt = InputStream.read(tempBuffer, 0, tempBuffer.size)) != -1) {
//                    if (cnt > 0) {
//                        sourceLine.write(tempBuffer, 0, cnt)
//                    }
//                }
//                //  sourceLine.drain();
//                // sourceLine.close();
//            } catch (e: Exception) {
//                println(e)
//                System.exit(0)
//            }
//
//        }
//    }
//
//    companion object {
//
//        @JvmStatic
//        fun main(args: Array<String>) {
//            VUServer().runVOIP()
//        }
//    }
//}}