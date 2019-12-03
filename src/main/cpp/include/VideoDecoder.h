//
// Created by bian on 2019/12/3.
//

#ifndef NDKPROJECT_VIDEODECODER_H
#define NDKPROJECT_VIDEODECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include "libavutil/frame.h"
#include <libswresample/swresample.h>

#define OUTPUT_CHANNELS 2

int64_t lastReadPacketTime;

//视频帧自定义结构体
typedef struct VideoFrame {
    int width;
    int height;
    float position;
    int format;
    unsigned char *data;

    VideoFrame() {
        width = 0;
        height = 0;
        position = -1;
        format = -1;
        data = NULL;
    }

    ~VideoFrame() {
        if (data == NULL) {
            free(data);
            data = NULL;
        }
    }
} VideoFrame;

//音频帧自定义结构体
typedef struct AudioPacket {
    float position;
    short *samples;
    int channel;
    int sampleRate;
    int format;

    AudioPacket() {
        position = -1;
        sampleRate = 0;
        format = -1;
        samples = NULL;
    }

    ~AudioPacket() {
        if (sampleRate == NULL) {
            free(samples);
            samples = NULL;
        }

    }
} AudioPacket;

class VideoDecoder {
private:
    AVFormatContext *avFormatContext;
    int audioStreamIndex;
    int videoStreamIndex;
    AVStream *audioStream;
    AVStream *videoStream;
    SwrContext *swrContext;
    AVFrame *audioFrame;
    AVPicture avPicture;
    SwsContext *swsContext;
    AVFrame *videoFrame;

public:

    VideoDecoder();

    int init(const char *source);

    static int interrupt_cb(void *ctx);

    AVPacket decodePacket();

    void destroy();

    bool audioCodecIsSupported(int foramt);

    void printError(int errCode);

    ~VideoDecoder();

    void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *packet);
};

#ifdef __cplusplus
}
#endif

#endif //NDKPROJECT_VIDEODECODER_H
