package com.jqueue.jniproject

import android.content.pm.ActivityInfo
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.ArrayMap
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat


open class BaseActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT
        window.addFlags(View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION)
        val originFlag = window.decorView.systemUiVisibility
        window.decorView.systemUiVisibility = originFlag or View
                .SYSTEM_UI_FLAG_LIGHT_STATUS_BAR or View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR
    }

    val runMap = ArrayMap<Int, () -> Unit>()
    private var requestCode: Int = 0
    fun hasPermission(vararg permission: String) = permission.asList().filter {
        ContextCompat.checkSelfPermission(
                this,
                it
        ) != PackageManager.PERMISSION_GRANTED
    }.isEmpty()

    fun requestPermission(vararg permissions: String, requestCode: Int, run: () -> Unit) {
        ActivityCompat.requestPermissions(this, permissions, requestCode)
        this.requestCode = requestCode
        runMap[requestCode] = run
    }

    override fun onRequestPermissionsResult(
            requestCode: Int,
            permissions: Array<out String>,
            grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode == this.requestCode && grantResults.asList().filter { it != PackageManager.PERMISSION_GRANTED }.isEmpty()) {
            runMap[requestCode]?.invoke()
            runMap.remove(requestCode)
        } else {

        }
    }
}
