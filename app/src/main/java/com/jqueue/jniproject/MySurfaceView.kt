package com.jqueue.jniproject

import android.content.Context
import android.graphics.PixelFormat
import android.util.AttributeSet
import android.view.SurfaceView

class MySurfaceView(context: Context,attributeSet: AttributeSet):SurfaceView(context,attributeSet) {
    init {
        holder.setFormat(PixelFormat.RGBA_8888)
    }
}