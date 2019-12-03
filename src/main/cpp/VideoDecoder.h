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
#include "3rdparty/ffmpeg/include/libavutil/frame.h"
#include <libswresample/swresample.h>

#define OUTPUT_CHANNELS 2

int64_t lastReadPacketTime;

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
