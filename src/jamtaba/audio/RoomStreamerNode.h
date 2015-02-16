#pragma once

#include "core/AudioNode.h"
#include <memory>
#include <QNetworkReply>
#include <QObject>
#include <QList>
#include <QFile>
#include <deque>
#include <vector>
#include <QMutex>
#include <QMutexLocker>

namespace Audio {

class Mp3Decoder;

class AbstractMp3Streamer : public QObject, public AudioNode{
   Q_OBJECT//allow signal/slots
public:
    explicit AbstractMp3Streamer(Audio::Mp3Decoder* decoder);
    ~AbstractMp3Streamer();
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
    virtual void stopCurrentStream();
    virtual void setStreamPath(QString streamPath);
private:
    static const int MAX_BYTES_PER_DECODING = 2048;

protected:
    QMutex mutex;
    Audio::Mp3Decoder* decoder;
    std::vector<std::deque<float>> samplesBuffer;
    QIODevice* device;
    void decodeBytesFromDevice(QIODevice* device, const unsigned int bytesToRead);
    virtual void initialize(QString streamPath) = 0;
};
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
class RoomStreamerNode : public AbstractMp3Streamer
{

Q_OBJECT//allow signal/slots

public:
    RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds=3);
    RoomStreamerNode(int bufferTimeInSeconds=3);
    ~RoomStreamerNode();

    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
    //virtual void stopCurrentStream();
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
    AudioFileStreamerNode(QString file);
    ~AudioFileStreamerNode();
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);

};

//++++++++++++++++++++++
class TestStreamerNode : public AbstractMp3Streamer{
private:
    OscillatorAudioNode* oscilator;
    bool playing;
protected:
    void initialize(QString streamPath);
public:
    TestStreamerNode();
    ~TestStreamerNode();
    void stopCurrentStream();
    void setStreamPath(QString streamPath);
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
};

}//namespace end
