#ifndef ROOM_STREAMER_NODE_H
#define ROOM_STREAMER_NODE_H

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "SamplesBufferResampler.h"

class QIODevice;

namespace Audio {

class Mp3Decoder;

class PublicRoomStreamDecoder : public QObject
{
    Q_OBJECT

public:
    PublicRoomStreamDecoder();
    ~PublicRoomStreamDecoder();
    void reset();
    int getSampleRate() const;
    void decode(QByteArray &bytesToDecode);

signals:
    void audioDecoded(const Audio::SamplesBuffer &decodedBuffer);

private:
    Audio::Mp3Decoder *mp3Decoder;
    void doDecode(QByteArray &bytesToDecode);

};

// +++++++++++++++++++++++++++++++++++++++++++++
class PublicRoomStreamerNode : public AudioNode
{
    Q_OBJECT

public:
    PublicRoomStreamerNode(const QUrl &streamPath = QUrl(""));
    ~PublicRoomStreamerNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiMessageBuffer &midiBuffer);
    bool needResamplingFor(int targetSampleRate) const;
    void setStreamPath(const QString &streamPath);
    void stopCurrentStream();
    inline bool isBuffering() const { return buffering; }
    inline bool isStreaming() const{ return streaming; }
    int getBufferingPercentage();
    int getSampleRate() const;

signals:
    void error(const QString &errorMsg);

private:
    QNetworkAccessManager *httpClient;
    QIODevice *device;

    PublicRoomStreamDecoder *decoder;

    SamplesBuffer bufferedSamples;
    SamplesBufferResampler resampler;

    QByteArray downloadedBytes;
    QMutex mutexDownloadedBytes;
    QMutex mutexBufferedSamples;

    bool buffering;
    bool streaming;

    qreal bufferedTime;

    int getSamplesToRender(int targetSampleRate, int outLenght);
    void initialize(const QString &streamPath);

    static const qreal BUFFER_TIME;

private slots:
    void handleNetworkError(QNetworkReply::NetworkError);
    void processDownloadedData();
    void appendDecodeAudio(const Audio::SamplesBuffer &decodedBuffer);
};

// ++++++++++++++++++++++
}// namespace

#endif
