package cs.sk.musicstreamer.room

import cs.sk.musicstreamer.connection.connectors.StreamingListener
import cs.sk.musicstreamer.connection.connectors.Switchable
import mu.KLogging
import org.springframework.stereotype.Service
import javax.sound.sampled.*


@Service
class MusicPlayer {

    companion object: KLogging()

    private lateinit var provider: Switchable
    private var audioLine: SourceDataLine? = null


    fun play() {
        provider.start()
    }


    fun stop() {
        provider.stop()
    }


    fun getStreamingListener() = StreamingListener(
            formatListener = { audioFormat -> prepareNewTrack(audioFormat) },
            onNewData = { audioData -> pushAudioData(audioData) },
            switch = { provider = it },
            onError = {}
    )


    @Synchronized
    fun prepareNewTrack(audioFormat: AudioFormat) {

        logger.debug("Received audio format of a new track\n$audioFormat")
        audioLine?.close()

        audioLine = AudioSystem.getLine(
                DataLine.Info(SourceDataLine::class.java, audioFormat)) as SourceDataLine

        audioLine?.open(audioFormat)
        audioLine?.start()
    }

    @Synchronized
    fun pushAudioData(audioData: ByteArray) {

        logger.debug("Received ${audioData.size} audio bytes...")
        audioLine?.write(audioData, 0, audioData.size)
    }
}

//
//class VUServer {
//
//    internal var InputStream: AudioInputStream
//    internal var sourceLine: SourceDataLine
//
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