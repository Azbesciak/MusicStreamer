package cs.sk.musicstreamer.room

import org.springframework.stereotype.Service
import java.io.File
import java.io.IOException
import javax.sound.sampled.*

@Service
class MusicConverter {

    @Throws(UnsupportedAudioFileException::class, IOException::class)
    fun mp3ToWav(mp3Data: File): File {
        val mp3Stream = AudioSystem.getAudioInputStream(mp3Data)
        val sourceFormat = mp3Stream.format
        val convertFormat = AudioFormat(AudioFormat.Encoding.PCM_SIGNED,
                sourceFormat.sampleRate, 16,
                sourceFormat.channels,
                sourceFormat.channels * 2,
                sourceFormat.sampleRate,
                false)
        val converted = AudioSystem.getAudioInputStream(convertFormat, mp3Stream)
        val tempFile = File.createTempFile("musicStreamer", "wavPort")
        AudioSystem.write(converted, AudioFileFormat.Type.WAVE, tempFile)
        return tempFile
    }
}
