//
// Created by bian on 2019/12/3.
//


#include <libswscale/swscale.h>
#include <libavutil/time.h>
#include "VideoDecoder.h"
#include "include/utils.h"

#define LOG_TAG "VideoDecoder"

VideoDecoder::VideoDecoder() {
    audioStreamIndex = -1;
    videoStreamIndex = -1;
    lastReadPacketTime = 0;
}

int VideoDecoder::init(const char *source) {
    LOGI("init");
    //ffmpeg注册协议，格式，编码解码器
    av_register_all();
    avfilter_register_all();
    avformat_network_init();

    avFormatContext = avformat_alloc_context();
    //注册回调
    avFormatContext->interrupt_callback.callback = interrupt_cb;
    //打开输入文件
    lastReadPacketTime = av_gettime();
    int result = avformat_open_input(&avFormatContext, source, NULL, NULL);
    if (result != 0) {
        printError(result);
        return result;
    }
    //获取流信息
    result = avformat_find_stream_info(avFormatContext, NULL);
    if (result != 0) {
        printError(result);
        return result;
    }
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        AVStream *avStream = avFormatContext->streams[i];
        if (AVMEDIA_TYPE_VIDEO == avStream->codecpar->codec_type) {
            videoStreamIndex = i;
        } else if (AVMEDIA_TYPE_AUDIO == avStream->codecpar->codec_type) {
            audioStreamIndex = i;
        } else {
            LOGW("Find other stream type=%d", avStream->codecpar->codec_type);
        }
    }
    if (audioStreamIndex == -1) {
        LOGE("AudioStream not found");
    }
    if (videoStreamIndex == -1) {
        LOGE("VideoStream not found");
    }
    if (audioStreamIndex == -1 && videoStreamIndex == -1) {
        LOGE("AVStream are NULL,return!");
        return -1;
    }
    //查找，打开解码器
    audioStream = avFormatContext->streams[audioStreamIndex];
    videoStream = avFormatContext->streams[videoStreamIndex];

    AVCodecParameters *audioCodecPara = audioStream->codecpar;
    //获取音频解码器
    AVCodec *audioCodec = avcodec_find_decoder(audioCodecPara->codec_id);
    if (!audioCodec) {
        LOGE("Can not find audio decoder");
        return -1;
    } else {
        //打开音频解码器
        result = avcodec_open2(audioStream->codec, audioCodec, NULL);
        if (result < 0) {
            printError(result);
            return result;
        }
    }

    AVCodecParameters *videoCodecPara = videoStream->codecpar;
    //获取视频解码器
    AVCodec *videoCodec = avcodec_find_decoder(videoCodecPara->codec_id);
    if (!videoCodec) {
        LOGE("Can not find video decoder");
        return -1;
    } else {
        //打开视频解码器
        result = avcodec_open2(videoStream->codec, videoCodec, NULL);
        if (result < 0) {
            printError(result);
            return result;
        }
    }

    //任意音频格式转为PCM才能播放(重采样初始化)
    if (audioCodecIsSupported(videoCodecPara->format)) {
        //音频解码
        swrContext = swr_alloc_set_opts(NULL, //重采样上下文，这里新建对象，传入NULL
                                        av_get_default_channel_layout(OUTPUT_CHANNELS),//输出的通道个数
                                        AV_SAMPLE_FMT_S16,//输出采样格式
                                        audioCodecPara->sample_rate,//输出采样率
                                        audioCodecPara->channel_layout,//输入通道个数
                                        static_cast<AVSampleFormat>(audioCodecPara->format),//输入采样格式
                                        audioCodecPara->sample_rate,//输入采样率
                                        0,
                                        NULL);
        if (!swrContext) {
            LOGE("alloc resampler failed!");
            return -1;
        }
        result = swr_init(swrContext);
        if (result < 0) {
            swr_free(&swrContext);
            printError(result);
            return result;
        }
        audioFrame = av_frame_alloc();
    }

    //视频格式转换初始化
    result = avpicture_alloc(&avPicture,
                             AV_PIX_FMT_YUV420P,
                             videoCodecPara->width,
                             videoCodecPara->height);
    if (result < 0) {
        printError(result);
        return result;
    }
    swsContext = sws_getCachedContext(NULL,//传入NULL，开辟新的空间
                                      videoCodecPara->width, //源视频宽度
                                      videoCodecPara->height,//源视频调试
                                      static_cast<AVPixelFormat>(videoCodecPara->format),//源视频像素格式
                                      videoCodecPara->width,//输出宽度
                                      videoCodecPara->height,//输出高度
                                      AV_PIX_FMT_YUV420P,//输出像素格式
                                      SWS_FAST_BILINEAR,//
                                      NULL, //源过滤器
                                      NULL, //输出过滤器
                                      NULL   //其它参数
    );
    videoFrame = av_frame_alloc();

    return 0;
}

/**
 * 超时回调函数
 * @param ctx
 * @return 1:超时  0：不超时
 */
int VideoDecoder::interrupt_cb(void *ctx) {
    LOGI("interrupt_cb");
    int timeout = 3;
    if (av_gettime() - lastReadPacketTime > timeout * 1000 * 1000) {
        return 1;
    }
    return 0;
}

void VideoDecoder::printError(int errCode) {
    char *errBuf = static_cast<char *>(malloc(1024));
    av_strerror(errCode, errBuf, 1024);
    LOGE("ffmpeg got err: %s", errBuf);
    free(errBuf);
}

VideoDecoder::~VideoDecoder() {

}

/**
 * 判断是否需要解码
 * @return
 */
bool VideoDecoder::audioCodecIsSupported(int foramt) {
    if (foramt == AV_SAMPLE_FMT_S16) {
        return true;
    }
    return false;
}

AVPacket VideoDecoder::decodePacket() {
    AVPacket avPacket;
    while (true) {
        //读取packet，再判断packet是Audio还是Video
        lastReadPacketTime = av_gettime();
        if (av_read_frame(avFormatContext, &avPacket) < 0) {
            LOGD("Read the end!");
            break;
        };
        if (avPacket.stream_index == audioStreamIndex) {
            //是Audio类型的AVPacket
            decode(audioStream->codec, audioFrame, &avPacket);
        } else if (avPacket.stream_index == videoStreamIndex) {
            //是Video类型的AVPacket
            decode(videoStream->codec, videoFrame, &avPacket);
        }
    }
    return AVPacket();
}

/**
 * 解码一个packet
 * @param dec_ctx 解码器上下文
 * @param frame 解码后存储的Frame
 * @param packet 要被解码的对象
 */
void VideoDecoder::decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *packet) {
    int ret = avcodec_send_packet(dec_ctx, packet);
    if (ret < 0) {
        printError(ret);
        return;
    }
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            LOGD("End decode this packet");
            break;
        } else if (ret < 0) {
            printError(ret);
            break;
        }
    }
}

void VideoDecoder::destroy() {
    if (swrContext) {
        swr_free(&swrContext);
    }
    if (audioFrame) {
        av_free(&audioFrame);
        audioFrame = NULL;
    }

    if (swsContext) {
        sws_freeContext(swsContext);
        swsContext = NULL;
    }
    if (videoFrame) {
        av_free(&videoFrame);
        videoFrame = NULL;
    }
    if (avFormatContext) {
        avformat_close_input(&avFormatContext);
        avFormatContext = NULL;
    }
    lastReadPacketTime = 0;
}


