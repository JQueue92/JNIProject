package com.jqueue.jniproject.player

import android.media.AudioAttributes
import android.media.AudioFormat
import android.media.AudioManager
import android.media.AudioTrack
import android.os.Build
import android.view.Surface

class VideoPlayer {
    companion object {
        init {
            System.loadLibrary("avcodec-56")
            System.loadLibrary("avdevice-56")
            System.loadLibrary("avfilter-5")
            System.loadLibrary("avformat-56")
            System.loadLibrary("avutil-54")
            System.loadLibrary("postproc-53")
            System.loadLibrary("swresample-1")
            System.loadLibrary("swscale-3")
            System.loadLibrary("native-lib")
        }
    }

    private var audioTrack: AudioTrack? = null

    fun createTrack(sampleRateInHz: Int, nb_channel: Int) {
        val channelConfig = when (nb_channel) {
            2 -> {
                AudioFormat.CHANNEL_IN_STEREO
            }
            else -> {
                AudioFormat.CHANNEL_IN_MONO
            }
        }

        val buffer_size = AudioTrack.getMinBufferSize(
            sampleRateInHz,
            channelConfig,
            AudioFormat.ENCODING_PCM_16BIT
        )
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            audioTrack = AudioTrack(
                AudioManager.STREAM_MUSIC, sampleRateInHz, channelConfig,
                AudioFormat.ENCODING_PCM_16BIT, buffer_size, AudioTrack.MODE_STREAM
            )
        } else {
            audioTrack = AudioTrack(
                AudioAttributes.Builder()
                    .setLegacyStreamType(AudioManager.STREAM_MUSIC)
                    .build(),
                AudioFormat.Builder()
                    .setChannelMask(channelConfig)
                    .setSampleRate(sampleRateInHz)
                    .setEncoding(AudioFormat.ENCODING_PCM_16BIT).build(),
                buffer_size,
                AudioTrack.MODE_STREAM,
                AudioManager.AUDIO_SESSION_ID_GENERATE
            )
        }
        audioTrack!!.play()
    }

    fun playTrack(byteArray: ByteArray, len: Int) {
        if (audioTrack != null && audioTrack!!.state != AudioTrack.PLAYSTATE_PLAYING) {
            audioTrack!!.write(byteArray, 0, len)
        }
    }

    external fun play(path: String, surface: Surface)
    external fun playAudio(path: String)
}