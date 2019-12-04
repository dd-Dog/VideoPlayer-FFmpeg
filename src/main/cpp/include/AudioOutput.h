//
// Created by bian on 2019/12/4.
//

#ifndef NDKPROJECT_AUDIOOUTPUT_H
#define NDKPROJECT_AUDIOOUTPUT_H

extern "C" {
#include <SLES/OpenSLES.h>
};

#define
class AudioOutput {
private:
    SLObjectItf engineObj;
    SLEngineItf engineEngine;
    SLObjectItf outputMixObj;
    SLObjectItf audioPlayerObj;
    SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf;
    SLPlayItf slPlayItf;

    void *dataBuffer;

public:
    AudioOutput(int channel, int sampleRate, int format);

    ~AudioOutput();

    int init(int channle, int sampleRate, int format);

    void playCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *pContext);

    SLuint32 getData();

    int start();

    int pause();

    int stop();

    void destroy();
};


#endif //NDKPROJECT_AUDIOOUTPUT_H
