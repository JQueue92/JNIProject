package com.jqueue.jniproject

import android.Manifest
import android.os.Bundle
import android.os.Environment
import android.util.Log
import com.jqueue.jniproject.player.VideoPlayer
import kotlinx.android.synthetic.main.activity_main.*
import java.io.File
import java.io.FileOutputStream
import kotlin.concurrent.thread

class MainActivity : BaseActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        if (!hasPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE, requestCode = 1) {
                play()
            }
        } else {
            play()
        }
    }

    fun play() {
        thread(true) {
            val file =
                File(getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS), "test.mp4")
            if (!file.exists()) {
                val fos = FileOutputStream(file)
                val inputStream = assets.open("test.mp4")
                val byteArray = ByteArray(1024)
                while (inputStream.read(byteArray) > 0) {
                    fos.write(byteArray, 0, byteArray.size)
                }
                fos.close()
                inputStream.close()
            }
            Log.d("MainActivity", "${file.exists()}\tpath=${file.absolutePath}")
            VideoPlayer().play(file.absolutePath, videoView.holder.surface)
            //VideoPlayer().playAudio(file.absolutePath)
        }
    }

}
