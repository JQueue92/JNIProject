package com.jqueue.jniproject.player

import android.view.Surface

class VideoPlayer {
    companion object{
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
    external fun play(path:String,surface: Surface)
}