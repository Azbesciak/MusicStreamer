package cs.sk.musicstreamer.room

import cs.sk.musicstreamer.connection.connectors.StreamingConnector
import cs.sk.musicstreamer.connection.connectors.StreamingListener
import cs.sk.musicstreamer.connection.connectors.Switchable
import org.springframework.stereotype.Service
import javax.sound.sampled.AudioFormat
import javax.swing.*
import java.awt.*
import java.awt.event.*
import java.io.*
import java.net.*
import javax.sound.sampled.*
import javax.sound.sampled.AudioSystem
import javax.sound.sampled.SourceDataLine
import javax.sound.sampled.DataLine
import javax.sound.sampled.AudioInputStream
import java.io.ByteArrayInputStream
import java.net.DatagramPacket
import java.net.DatagramSocket
import javax.sound.sampled.TargetDataLine
import java.io.ByteArrayOutputStream


@Service
class MusicPlayer {
    private var audioFormat: AudioFormat? = null
    private var targetDataLine: TargetDataLine? = null
    private var byteOutputStream = ByteArrayOutputStream()
    private lateinit var provider: Switchable
    fun play() {
    }

    fun stop() {

    }

    fun getStreamingListener() = StreamingListener(
            onNewData = { audioData ->
                val byteInputStream = ByteArrayInputStream(audioData)
                val inputStream = AudioInputStream(byteInputStream, audioFormat, (audioData.size / audioFormat!!.frameSize).toLong())
                val dataLineInfo = DataLine.Info(SourceDataLine::class.java, audioFormat)
                val sourceLine = AudioSystem.getLine(dataLineInfo) as SourceDataLine
                sourceLine.open(audioFormat)
                sourceLine.start()
            },
            onError = {},
            formatListener = { audioFormat = it },
            switch = { provider = it }
    )


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