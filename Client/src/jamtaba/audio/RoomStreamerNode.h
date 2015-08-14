#pragma once

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>
#include <deque>
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

private:
    static const int MAX_BYTES_PER_DECODING;

protected:
    Audio::FaderProcessor faderProcessor;//used to apply fade in in stream
    //QMutex mutex;
    Audio::Mp3Decoder* decoder;

    QIODevice* device;
    void decode(const unsigned int maxBytesToDecode);
    QByteArray bytesToDecode;
    virtual void initialize(QString streamPath);
    bool streaming;
    SamplesBuffer bufferedSamples;
};
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
class RoomStreamerNode : public AbstractMp3Streamer
{

Q_OBJECT//allow signal/slots

public:
    RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds=3);
    explicit RoomStreamerNode(int bufferTimeInSeconds=3);
    ~RoomStreamerNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer);
    //virtual void stopCurrentStream();
    virtual bool needResamplingFor(int targetSampleRate) const;
protected:
    void initialize(QString streamPath);
private:
    QNetworkAccessManager* httpClient;
    bool buffering;
    int bufferSize;

private slots:
    void on_reply_error(QNetworkReply::NetworkError);
    void on_reply_read();
    void on_reply_finished();
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
/*
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
*/

}//namespace end
