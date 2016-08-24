#ifndef ROOM_STREAMER_NODE_H
#define ROOM_STREAMER_NODE_H

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "SamplesBufferResampler.h"

class QIODevice;

namespace Audio {
class Mp3Decoder;

// +++++++++++++++++++++++++++++++++++++++++++++
class NinjamRoomStreamerNode : public AudioNode
{
    Q_OBJECT

public:
    NinjamRoomStreamerNode(const QUrl &streamPath = QUrl(""));
    ~NinjamRoomStreamerNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiMessageBuffer &midiBuffer);
    bool needResamplingFor(int targetSampleRate) const;
    void setStreamPath(const QString &streamPath);
    void stopCurrentStream();
    inline bool isBuffering() const { return buffering; }
    inline bool isStreaming() const{ return streaming; }
    int getBufferingPercentage() const;
    int getSampleRate() const;

signals:
    void error(const QString &errorMsg);

private:
    QNetworkAccessManager *httpClient;
    QIODevice *device;
    Audio::Mp3Decoder *decoder;
    SamplesBuffer bufferedSamples;
    SamplesBufferResampler resampler;
    QByteArray bytesToDecode;
    bool buffering;
    bool streaming;

    int getSamplesToRender(int targetSampleRate, int outLenght);
    void initialize(const QString &streamPath);
    void decode(const unsigned int maxBytesToDecode);

    static const int BUFFER_SIZE;
    static const int MAX_BYTES_PER_DECODING;

private slots:
    void handleNetworkError(QNetworkReply::NetworkError);
    void processDownloadedData();
};

// ++++++++++++++++++++++
}// namespace

#endif
