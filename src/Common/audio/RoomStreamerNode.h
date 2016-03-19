#ifndef ROOM_STREAMER_NODE_H
#define ROOM_STREAMER_NODE_H

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
// #include <deque>
#include "SamplesBufferResampler.h"

class QIODevice;

namespace Audio {
class Mp3Decoder;

class AbstractMp3Streamer : public AudioNode
{
    Q_OBJECT
public:
    explicit AbstractMp3Streamer(Audio::Mp3Decoder *decoder);
    ~AbstractMp3Streamer();
    virtual void processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                                  int sampleRate, const Midi::MidiBuffer &midiBuffer);
    virtual void stopCurrentStream();
    virtual void setStreamPath(const QString &streamPath);
    inline bool isStreaming() const
    {
        return streaming;
    }

    virtual int getSampleRate() const;
    virtual bool needResamplingFor(int targetSampleRate) const;

    virtual bool isBuffering() const  = 0;
    virtual int getBufferingPercentage() const = 0;

signals:
    void error(const QString &errorMsg);
private:
    static const int MAX_BYTES_PER_DECODING;

protected:
    Audio::Mp3Decoder *decoder;

    QIODevice *device;
    void decode(const unsigned int maxBytesToDecode);
    QByteArray bytesToDecode;
    virtual void initialize(const QString &streamPath);
    bool streaming;
    SamplesBuffer bufferedSamples;
    SamplesBufferResampler resampler;

    int getSamplesToRender(int targetSampleRate, int outLenght);
};

// +++++++++++++++++++++++++++++++++++++++++++++
class NinjamRoomStreamerNode : public AbstractMp3Streamer
{
    Q_OBJECT

public:
    NinjamRoomStreamerNode(const QUrl &streamPath = QUrl(""));
    ~NinjamRoomStreamerNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiBuffer &midiBuffer);
    virtual bool needResamplingFor(int targetSampleRate) const;

    inline bool isBuffering() const override { return buffering; }

    int getBufferingPercentage() const override;

protected:
    void initialize(const QString &streamPath);
private:
    QNetworkAccessManager *httpClient;
    bool buffering;

    static const int BUFFER_SIZE;

private slots:
    void on_reply_error(QNetworkReply::NetworkError);
    void on_reply_read();
};
// ++++++++++++++++++++++++++++
class AudioFileStreamerNode : public AbstractMp3Streamer
{
protected:
    void initialize(const QString &streamPath);

public:
    explicit AudioFileStreamerNode(const QString &file);
    ~AudioFileStreamerNode();
    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiBuffer &midiBuffer);
};

// ++++++++++++++++++++++
}// namespace end

#endif
