#ifndef FFMPEGMUXER_H
#define FFMPEGMUXER_H

#include <QObject>
#include <QImage>
#include <QSize>
#include <QFile>
#include <QDebug>
#include <QThreadPool>

#include "FFMpegCommon.h"

#include <memory>

// adapted from FFMpeg muxing.c example

class FFMpegMuxer : public QObject
{
    Q_OBJECT

public:
    FFMpegMuxer(QObject *parent = nullptr);
    ~FFMpegMuxer();

    void encodeImage(const QImage &image, bool async = true);
    void encodeAudioFrame();

    void setVideoResolution(const QSize &resolution);
    QSize getVideoResolution() const;

    void setVideoFrameRate(qreal frameRate);

    enum VideoQuality
    {
        VideoQualityLow    = 64000,     // 64 kbps
        VideoQualityMedium = 96000,     // 96 kbps
        VideoQualityHigh   = 128000,    // 128 kbps
        VideoQualityBest   = 400000     // 400 kbps (default value in ffmpeg examples)
    };


    /**
     * @brief Set video bitrate
     * @param quality - Pre-defined bit rate values in VideoQuality enum
     */
    void setVideoQuality(VideoQuality quality);

    int64_t getCurrentVideoPresentationTimeStamp() const;

signals:
    void dataEncoded(const QByteArray &data, bool isFirstPacket);
    void encodingFinished();

public slots:
    void startNewInterval();

private:

    void finishCurrentInterval();
    bool prepareToEncodeNewInterval();

    bool addVideoStream(AVCodecID codecID, AVDictionary **opts);

    bool openVideoCodec(AVCodec *codec, AVDictionary **opts);
    void openAudioCodec(AVCodec *codec);

    bool doEncodeVideoFrame(const QImage &image);
    bool doEncodeAudioFrame(); // TODO add a SamplesBuffer parameter

    AVFrame *allocAudioFrame(enum AVSampleFormat sampleFormat, uint64_t channelLayout, int sampleRate, int nbSamples);
    AVFrame *allocPicture(enum AVPixelFormat pixelFormat, int width, int height);
    void imageToYuvPicture(const QImage &image, AVFrame *picture, int width, int height);
    void fillFrameWithImageData(const QImage &image);
    void RGBtoYUV420P(const uint8_t *bufferRGB, uint8_t *bufferYUV, uint rgbIncrement, bool swapRGB, int width, int height);

    void initialize();

    bool encodeVideo;
    bool encodeAudio;

    int64_t videoPts; // pts (presentation time stamp) of the next frame that will be generated
    quint64 encodedFrames;

    // internal streams
    class VideoOutputStream;
    class AudioOutputStream;

    //std::unique_ptr<VideoOutputStream> videoStream;
    std::unique_ptr<AudioOutputStream> audioStream;

    AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVFrame *tempFrame;
    SwsContext *swsContext;

    QSize videoResolution;
    qreal videoFrameRate;
    uint videoBitRate;

    bool initialized;
    bool startNewIntervalRequested;

    QThreadPool threadPool;
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
