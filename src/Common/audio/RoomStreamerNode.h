#ifndef ROOM_STREAMER_NODE_H
#define ROOM_STREAMER_NODE_H

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "SamplesBufferResampler.h"

class QIODevice;

namespace audio {

class Mp3Decoder;

class AbstractMp3Streamer : public AudioNode
{
    Q_OBJECT

public:
    explicit AbstractMp3Streamer(audio::Mp3Decoder *decoder);
    virtual ~AbstractMp3Streamer();
    void processReplacing(const audio::SamplesBuffer &in, audio::SamplesBuffer &out,
                          int sampleRate, std::vector<midi::MidiMessage> &midiBuffer) override;
    virtual void stopCurrentStream();
    virtual void setStreamPath(const QString &streamPath);
    bool isStreaming() const;

    virtual int getSampleRate() const;
    virtual bool needResamplingFor(int targetSampleRate) const;

    virtual bool isBuffering() const  = 0;
    virtual int getBufferingPercentage() const = 0;

signals:
    void error(const QString &errorMsg);

private:
    static const int MAX_BYTES_PER_DECODING;

protected:
    audio::Mp3Decoder *decoder;

    QIODevice *device;
    void decode(const unsigned int maxBytesToDecode);
    QByteArray bytesToDecode;
    virtual void initialize(const QString &streamPath);
    bool streaming;
    SamplesBuffer bufferedSamples;
    SamplesBufferResampler resampler;

    int getSamplesToRender(int targetSampleRate, int outLenght);
};

inline bool AbstractMp3Streamer::isStreaming() const
{
    return streaming;
}

// +++++++++++++++++++++++++++++++++++++++++++++

class NinjamRoomStreamerNode : public AbstractMp3Streamer
{
    Q_OBJECT

public:
    explicit NinjamRoomStreamerNode(const QUrl &streamPath = QUrl(""));
    ~NinjamRoomStreamerNode();

    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<midi::MidiMessage> &midiBuffer) override;
    bool needResamplingFor(int targetSampleRate) const override;

    bool isBuffering() const override;

    int getBufferingPercentage() const override;

protected:
    void initialize(const QString &streamPath) override;

private:
    QNetworkAccessManager httpClient;
    bool buffering;

    static const int BUFFER_SIZE;

private slots:
    void on_reply_error(QNetworkReply::NetworkError);
    void on_reply_read();
};

inline bool NinjamRoomStreamerNode::isBuffering() const
{
    return buffering;
}

// ++++++++++++++++++++++++++++

class AudioFileStreamerNode : public AbstractMp3Streamer

{
protected:
    void initialize(const QString &streamPath) override;

public:
    explicit AudioFileStreamerNode(const QString &file);
    ~AudioFileStreamerNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  std::vector<midi::MidiMessage> &midiBuffer) override;
};

} // namespace end

#endif
