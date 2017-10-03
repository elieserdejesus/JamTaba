#ifndef FFMPEGMUXER_H
#define FFMPEGMUXER_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <QFile>
#include <QMutex>
#include <QThread>
#include <QDebug>
#include <QWaitCondition>

#include "FFMpegCommon.h"

// adapted from FFMpeg muxing.c example

class FFMpegMuxer : public QObject
{
    Q_OBJECT

public:
    FFMpegMuxer();
    ~FFMpegMuxer();

    void encodeImage(const QImage &image);
    void encodeAudioFrame();

    void setVideoResolution(const QSize &resolution);
    QSize getVideoResolution() const;

    void setVideoFrameRate(qreal frameRate);

    enum VideoQuality
    {
        LOW_VIDEO_QUALITY = 32000,    // 32 kbps
        MEDIUM_VIDEO_QUALITY = 64000, // 64 kbps
        HIGHT_VIDEO_QUALITY = 128000, // 128 kbps
        BEST_VIDEO_QUALITY = 400000   // 400 kbps (default value in ffmpeg examples)
    };


    /**
     * @brief Set video bitrate
     * @param quality - Pre-defined bit rate values in VideoQuality enum
     */
    void setVideoQuality(VideoQuality quality);

    int64_t getCurrentVideoPresentationTimeStamp() const;

signals:
    void dataEncoded(const QByteArray &data, bool isFirstPacket);

public slots:
    void startNewInterval();

private:

    void finishCurrentInterval();
    bool prepareToEncodeNewInterval();

    void addAudioStream(AVCodecID codecID);
    bool addVideoStream(AVCodecID codecID);

    bool openVideoCodec(AVCodec *codec);
    void openAudioCodec(AVCodec *codec);

    bool doEncodeVideoFrame(const QImage &image);
    bool doEncodeAudioFrame(); // TODO add a SamplesBuffer parameter

    int writeFrame(const AVRational *time_base, AVStream *stream, AVPacket *packet);

    AVFrame *allocAudioFrame(enum AVSampleFormat sampleFormat, uint64_t channelLayout, int sampleRate, int nbSamples);
    AVFrame *allocPicture(enum AVPixelFormat pixelFormat, int width, int height);
    void imageToYuvPicture(const QImage &image, AVFrame *picture, int width, int height);
    void fillFrameWithImageData(const QImage &image);
    void RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint rgbIncrement, bool swapRGB, int width, int height);

    void initialize();

    // avio callback
    static int writeCallback(void *instancePointer, uint8_t *buffer, int bufferSize);

    bool encodeVideo;
    bool encodeAudio;

    int64_t videoPts; // pts (presentation time stamp) of the next frame that will be generated

    // internal streams
    class VideoOutputStream;
    class AudioOutputStream;
    VideoOutputStream *videoStream;
    AudioOutputStream *audioStream;

    AVFormatContext *formatContext;
    AVIOContext *avioContext;

    unsigned char *buffer; // avio buffer used in callback

    QSize videoResolution;
    qreal videoFrameRate;
    uint videoBitRate;

    bool initialized;
    bool startNewIntervalRequested;

};

inline void FFMpegMuxer::setVideoQuality(VideoQuality quality)
{
    this->videoBitRate = static_cast<uint>(quality);
}

inline void FFMpegMuxer::setVideoFrameRate(qreal frameRate)
{
    this->videoFrameRate = frameRate;
}

inline int64_t FFMpegMuxer::getCurrentVideoPresentationTimeStamp() const
{
    return videoPts;
}

inline void FFMpegMuxer::setVideoResolution(const QSize &resolution)
{
    qDebug() << "Setting video resolution to" << resolution;

    this->videoResolution = resolution;
}

inline QSize FFMpegMuxer::getVideoResolution() const
{
    return videoResolution;
}

#endif // FFMPEGMUXER_H
