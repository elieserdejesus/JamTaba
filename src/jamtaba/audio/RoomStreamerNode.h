#pragma once

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>
#include <deque>
#include "SamplesBufferResampler.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(roomStreamer)

class QIODevice;

namespace Audio {

class Mp3Decoder;

class AbstractMp3Streamer : public QObject, public AudioNode{
   Q_OBJECT//allow signal/slots
public:
    explicit AbstractMp3Streamer(Audio::Mp3Decoder* decoder);
    ~AbstractMp3Streamer();
    virtual void processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer);
    virtual void stopCurrentStream();
    virtual void setStreamPath(QString streamPath);
    inline bool isStreaming() const{return streaming;}
    virtual int getSampleRate() const;
    virtual bool needResamplingFor(int targetSampleRate) const;
signals:
    void error(QString errorMsg);
private:
    static const int MAX_BYTES_PER_DECODING;
    //circular buffer

protected:
    //Audio::FaderProcessor faderProcessor;//used to apply fade in in stream
    //QMutex mutex;
    Audio::Mp3Decoder* decoder;

    QIODevice* device;
    void decode(const unsigned int maxBytesToDecode);
    QByteArray bytesToDecode;
    virtual void initialize(QString streamPath);
    bool streaming;
    SamplesBuffer bufferedSamples;
    SamplesBufferResampler resampler;

    int getSamplesToRender(int targetSampleRate, int outLenght);
};
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
class NinjamRoomStreamerNode : public AbstractMp3Streamer
{

Q_OBJECT//allow signal/slots

public:
    NinjamRoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds=3);
    explicit NinjamRoomStreamerNode(int bufferTimeInSeconds=3);
    ~NinjamRoomStreamerNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer);
    virtual bool needResamplingFor(int targetSampleRate) const;
protected:
    void initialize(QString streamPath);
private:
    QNetworkAccessManager* httpClient;
    //bool buffering;
    int bufferTime;//in seconds
    bool buffering;

private slots:
    void on_reply_error(QNetworkReply::NetworkError);
    void on_reply_read();
    //void on_reply_finished();
};
//++++++++++++++++++++++++++++
class AudioFileStreamerNode : public AbstractMp3Streamer
{
    //Q_OBJECT//allow signal/slots

protected:
    void initialize(QString streamPath);

public:
    explicit AudioFileStreamerNode(QString file);
    ~AudioFileStreamerNode();
    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer);

};

//++++++++++++++++++++++

class TestStreamerNode : public AbstractMp3Streamer{
private:
    OscillatorAudioNode* oscilator;
    bool playing;
protected:
    void initialize(QString streamPath);
public:
    TestStreamerNode(int sampleRate);
    ~TestStreamerNode();
    void stopCurrentStream();
    void setStreamPath(QString streamPath);
    virtual void processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer);
};


}//namespace end
