//
// Created by bian on 2019/12/4.
//

#include <utils.h>
#include <SLES/OpenSLES_Android.h>

#define LOG_TAG "AudioOutput"

#include "include/AudioOutput.h"

AudioOutput::AudioOutput(int channel, int sampleRate, int format) {
    init(channel, sampleRate, format);
}

int AudioOutput::init(int channel, int sampleRate, int format) {
    LOGI("init");
    SLresult sLresult = slCreateEngine(&engineObj, 0, 0, 0, 0, 0);
    (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
    (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engineEngine);

    //获取输出混音对象
    SLInterfaceID ids[] = {SL_IID_VOLUME};
    SLboolean reqs[] = {SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObj, 0, ids, reqs);
    (*outputMixObj)->Realize(outputMixObj, SL_BOOLEAN_FALSE);

    //locator:{type,num}
    SLDataLocator_AndroidSimpleBufferQueue inputLocator = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                           2};
    SLDataFormat_PCM inputFormat = {SL_DATAFORMAT_PCM,         //指定PCM格式
                                    2,                         //通道个数
                                    SL_SAMPLINGRATE_44_1,      //采样率
                                    SL_PCMSAMPLEFORMAT_FIXED_16,//采样精度
                                    SL_PCMSAMPLEFORMAT_FIXED_16,//窗口大小
                                    SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,//通道掩码
                                    SL_BYTEORDER_LITTLEENDIAN  //字节序：小端
    };
    //输入管道结构体：{资源定位器，输入格式}
    SLDataSource inputSource = {&inputLocator, &inputFormat};

    SLDataLocator_OutputMix outputMixLocator = {SL_DATALOCATOR_OUTPUTMIX, outputMixObj};
    //输出管道：{资源定位器，输出格式}
    SLDataSink outputSink = {&outputMixLocator, NULL};

    //创建Audio播放管理器
    const SLInterfaceID outputInterfaces[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean requireds[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_FALSE};
    (*engineEngine)->CreateAudioPlayer(engineEngine,
                                       &audioPlayerObj,
                                       &inputSource,
                                       &outputSink,
                                       1,
                                       outputInterfaces,
                                       requireds
    );
    (*audioPlayerObj)->Realize(audioPlayerObj, SL_BOOLEAN_FALSE);

    //获取音频数据对象
    (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                    &slAndroidSimpleBufferQueueItf);
    //获取播放对象
    (*audioPlayerObj)->GetInterface(audioPlayerObj, SL_IID_PLAY, &slPlayItf);

    //设置播放回调,往音频数据对象中填充数据
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(slAndroidSimpleBufferQueueItf,
                                                       playCallback,
                                                       this);

    return 0;
}

void AudioOutput::playCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *pContext) {
    SLuint32 size = getData();
    (*slAndroidSimpleBufferQueueItf)->Enqueue(slAndroidSimpleBufferQueueItf, dataBuffer, size);
}

SLuint32 AudioOutput::getData() {

    return 0;
}

int AudioOutput::start() {
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);
    playCallback(slAndroidSimpleBufferQueueItf, this);
    return 0;
}

int AudioOutput::pause() {
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PAUSED);
    return 0;
}

int AudioOutput::stop() {
    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_STOPPED);
    return 0;
}

void AudioOutput::destroy() {
    (*audioPlayerObj)->Destroy(audioPlayerObj);
    (*engineObj)->Destroy(engineObj);
    (*outputMixObj)->Destroy(outputMixObj);
}

AudioOutput::~AudioOutput() {

}




