package com.jqueue.jniproject

class Native {
    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    external fun stringFromJNI(): String
    external fun test1(p1:String,p2:Int)
    external fun testF(p1:Long,p2:Float,p3:Double,p4:Char,p5:Short,p6:Byte)
    external  fun getNativeVersion():Int
    external fun hello()
}
