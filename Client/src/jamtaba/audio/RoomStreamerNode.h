#pragma once

#include "core/AudioNode.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QObject>
#include <deque>

class QIODevice;

namespace Audio {

class Mp3Decoder;

class AbstractMp3Streamer : public QObject, public AudioNode{
   Q_OBJECT//allow signal/slots
public:
    explicit AbstractMp3Streamer(Audio::Mp3Decoder* decoder);
    ~AbstractMp3Streamer();
    virtual void processReplacing(SamplesBuffer &in, SamplesBuffer &out);
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
    std::vector<std::deque<float>> samplesBuffer;
    QIODevice* device;
    void decodeBytesFromDevice(QIODevice* device, const unsigned int bytesToRead);
    virtual void initialize(QString streamPath);
    bool streaming;
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

    virtual void processReplacing(SamplesBuffer &in, SamplesBuffer &out);
    //virtual void stopCurrentStream();
    virtual bool needResamplingFor(int targetSampleRate) const;
protected:
    void initialize(QString streamPath);
private:
    QNetworkAccessManager httpClient;
    bool buffering;
    int bufferSize;


    //QByteArray byteArray;
private slots:
    void reply_error(QNetworkReply::NetworkError);
    void reply_read();
};
//++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++
class AudioFileStreamerNode : public AbstractMp3Streamer
{
Q_OBJECT//allow signal/slots
    //Q_OBJECT//allow signal/slots

protected:
    void initialize(QString streamPath);

public:
    explicit AudioFileStreamerNode(QString file, int sampleRate);
    ~AudioFileStreamerNode();
    virtual void processReplacing(SamplesBuffer &in, SamplesBuffer &out);

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
    virtual void processReplacing(SamplesBuffer &in, SamplesBuffer &out);
};

}//namespace end
