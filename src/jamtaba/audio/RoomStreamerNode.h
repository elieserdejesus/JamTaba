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

private:
    static const int MAX_BYTES_PER_DECODING = 2048;

protected:
    QMutex mutex;
    Audio::Mp3Decoder* decoder;
    std::vector<std::deque<float>> samplesBuffer;
    QIODevice* device;
    void decodeBytesFromDevice(QIODevice* device, const unsigned int bytesToRead);

};
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++
class RoomStreamerNode : public AbstractMp3Streamer
{

Q_OBJECT//allow signal/slots

public:
    RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds=10);
    ~RoomStreamerNode();
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
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

public:
    AudioFileStreamerNode(QString file);
    ~AudioFileStreamerNode();
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);

};

}
