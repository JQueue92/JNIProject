#include <jni.h>
#include <string>
#include <android/log.h>
#include <android/native_window_jni.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif


#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"NativeLog",FORMAT,##__VA_ARGS__)
//java/kotlin class
#define REGISTER_CLASS_PATH "com/jqueue/jniproject/player/VideoPlayer"

using namespace std;

void play(JNIEnv *env, jobject mInstance, jstring path, jobject surface) {
    const char *url = env->GetStringUTFChars(path, JNI_FALSE);
    //注册FFmpeg组件获取上下文
    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) < 0) {
        LOGE("打开文件失败");
        return;
    }
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("获取文件内容失败");
        return;
    }
    //获取到视频内容后找到视频流
    int video_index = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_index = i;
        }
    }
    LOGE("成功找到视频流:%d", video_index);
    //对视频流就行解码
    //获取解码器上下文
    AVCodecContext *avCodecContext = avFormatContext->streams[video_index]->codec;
    //获取解码器
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    //打开解码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
        LOGE("解码器打开失败");
        return;
    }
    //申请AVPacket
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));
    av_init_packet(packet);
    //申请AVFrame
    AVFrame *frame = av_frame_alloc();
    AVFrame *rgb_frame = av_frame_alloc();

    //缓存区
    uint8_t *out_buffer = (uint8_t *) av_malloc(avpicture_get_size(AV_PIX_FMT_RGBA,
                                                                   avCodecContext->width,
                                                                   avCodecContext->height));
    //与缓存区相关联，设置rgb_frame缓存区
    avpicture_fill((AVPicture *) rgb_frame, out_buffer, AV_PIX_FMT_RGBA, avCodecContext->width,
                   avCodecContext->height);
    SwsContext *swsContext = sws_getContext(avCodecContext->width, avCodecContext->height,
                                            avCodecContext->pix_fmt,
                                            avCodecContext->width, avCodecContext->height,
                                            AV_PIX_FMT_RGBA,
                                            SWS_BICUBIC, NULL, NULL, NULL);
    //取到nativewindow
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    if (nativeWindow == 0) {
        LOGE("nativewindow取到失败");
        return;
    }
    //视频缓冲区
    ANativeWindow_Buffer native_outBuffer;
    int frameCount;
    int h = 0;
    LOGE("解码 ");
    while (av_read_frame(avFormatContext, packet) >= 0) {
        LOGE("解码 %d", packet->stream_index);
        LOGE("VINDEX %d", video_index);
        if (packet->stream_index == video_index) {
            LOGE("解码 hhhhh");
            //如果是视频流;
            //解码
            avcodec_decode_video2(avCodecContext, frame, &frameCount, packet);
            LOGE("解码中....  %d", frameCount);
            if (frameCount) {
                LOGE("转换并绘制");
                //说明有内容
                //绘制之前配置nativewindow
                ANativeWindow_setBuffersGeometry(nativeWindow, avCodecContext->width,
                                                 avCodecContext->height, WINDOW_FORMAT_RGBA_8888);
                //上锁
                ANativeWindow_lock(nativeWindow, &native_outBuffer, NULL);
                //转换为rgb格式
                sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                          frame->height, rgb_frame->data,
                          rgb_frame->linesize);
                //  rgb_frame是有画面数据
                uint8_t *dst = (uint8_t *) native_outBuffer.bits;
                //拿到一行有多少个字节 RGBA
                int destStride = native_outBuffer.stride * 4;
                //像素数据的首地址
                uint8_t *src = rgb_frame->data[0];
                //实际内存一行数量
                int srcStride = rgb_frame->linesize[0];
                //int i=0;
                for (int i = 0; i < avCodecContext->height; ++i) {
                    //将rgb_frame中每一行的数据复制给nativewindow
                    memcpy(dst + i * destStride, src + i * srcStride, srcStride);
                }
                //解锁
                ANativeWindow_unlockAndPost(nativeWindow);
                usleep(1000 * 16);
            }
        }
        av_free_packet(packet);
    }
    //释放
    ANativeWindow_release(nativeWindow);
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    avcodec_close(avCodecContext);
    avformat_free_context(avFormatContext);
    env->ReleaseStringUTFChars(path, url);
}

void playAudio(JNIEnv *env, jobject mInstance, jstring path) {
    av_register_all();
    AVFormatContext *avFormatContext = avformat_alloc_context();
    const char *url = env->GetStringUTFChars(path, JNI_FALSE);
    if (avformat_open_input(&avFormatContext, url, NULL, NULL) < 0) {//打开文件失败
        LOGE("打开文件失败");
        return;
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("查找媒体文件信息失败");
        return;
    }

    int audio_index = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_index = i;//找到音频流
        }
    }

    //解码
    AVCodecContext *avCodecContext = avFormatContext->streams[audio_index]->codec;
    AVCodec *avCodec = avcodec_find_decoder(avCodecContext->codec_id);
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {//打开解码器
        LOGE("打开解码器失败");
        return;
    }

    AVPacket *avPacket = (AVPacket *) av_malloc(sizeof(AVPacket));//解码前数据
    AVFrame *frame = av_frame_alloc();//解码后数据
    SwrContext *swrContext = swr_alloc();

    uint8_t *out_buffer = (uint8_t *) av_malloc(44100 * 2);

    uint64_t out_ch_layout = AV_CH_LAYOUT_STEREO;

    AVSampleFormat out_format = AV_SAMPLE_FMT_S16;

    int out_sample_rate = avCodecContext->sample_rate;

    swr_alloc_set_opts(swrContext, out_ch_layout, out_format, out_sample_rate,
                       avCodecContext->channel_layout, avCodecContext->sample_fmt,
                       avCodecContext->sample_rate, 0, NULL);

    swr_init(swrContext);

    int out_channel_nb = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);

    jclass videoPlayer = env->GetObjectClass(mInstance);

    jmethodID createTrack = env->GetMethodID(videoPlayer, "createTrack", "(II)V");

    env->CallVoidMethod(mInstance, createTrack, 44100, out_channel_nb);

    jmethodID playTrack = env->GetMethodID(videoPlayer, "playTrack", "([BI)V");
    LOGE("AudioTrack初始化完成解码");
    int got_frame;
    while (av_read_frame(avFormatContext, avPacket) >= 0) {
        if (avPacket->stream_index == audio_index) {
            avcodec_decode_audio4(avCodecContext, frame, &got_frame, avPacket);
            if (got_frame) {
                swr_convert(swrContext, &out_buffer, 44100 * 2, (const uint8_t **) frame->data,
                            frame->nb_samples);
                int size = av_samples_get_buffer_size(NULL, out_channel_nb, frame->nb_samples,
                                                      AV_SAMPLE_FMT_S16, 1);
                jbyteArray audio_sample_array = env->NewByteArray(size);
                env->SetByteArrayRegion(audio_sample_array, 0, size, (const jbyte *) out_buffer);
                env->CallVoidMethod(mInstance, playTrack, audio_sample_array, size);
                env->DeleteLocalRef(audio_sample_array);
            }
        }
    }
    av_frame_free(&frame);
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);
    env->ReleaseStringUTFChars(path, url);
}

JNINativeMethod methods[] = {
        {"play",      "(Ljava/lang/String;Landroid/view/Surface;)V", (void *) play},
        {"playAudio", "(Ljava/lang/String;)V",                       (void *) playAudio}
};

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    } else {
        jclass jclazz = env->FindClass(REGISTER_CLASS_PATH);
        if (jclazz) {
            jint ok = env->RegisterNatives(jclazz, methods, sizeof(methods) / sizeof(*methods));
            env->DeleteLocalRef(jclazz);
            return ok == 0 ? JNI_VERSION_1_6 : JNI_ERR;
        }
        return JNI_ERR;
    }
}
