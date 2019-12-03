//
// Created by bian on 2019/12/3.
//

#include "include/com_flyscale_chapter_5_NativeVideoPlayer.h"
#include "include/utils.h"
#include "VideoDecoder.h"

#define LOG_TAG "NativeVideoPlayer"

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL Java_com_flyscale_chapter_15_NativeVideoPlayer_getStringFromJNI
        (JNIEnv *env, jobject jobj, jstring jstr) {
    const char* jniStr = env->GetStringUTFChars(jstr, NULL);
    LOGI("Receive:%s", jniStr);
    env->ReleaseStringUTFChars(jstr, jniStr);
    return env->NewStringUTF("Hello,I am JNI!");
}

VideoDecoder *videoDecoder;
JNIEXPORT void JNICALL Java_com_flyscale_chapter_15_NativeVideoPlayer_test
        (JNIEnv *env, jobject jobj){
    videoDecoder = new VideoDecoder();
    videoDecoder->init("");
}

#ifdef __cplusplus
}
#endif
