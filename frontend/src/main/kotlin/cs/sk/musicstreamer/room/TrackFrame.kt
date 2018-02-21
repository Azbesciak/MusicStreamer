package cs.sk.musicstreamer.room

class TrackFrame (
        val sampleRate: Float,
        val sampleInButs: Int,
        val channels: Int,
        val bigEndian: Boolean,
        val signed: Boolean
)