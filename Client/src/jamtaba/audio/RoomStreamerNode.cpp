#include "RoomStreamerNode.h"
#include "codec.h"
#include "core/AudioDriver.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrl>
#include <QNetworkReply>
#include <QDebug>
#include <QBuffer>
#include <QObject>
#include <QDateTime>
#include <QWaitCondition>
#include <cmath>

using namespace Audio;

const int AbstractMp3Streamer::MAX_BYTES_PER_DECODING;// = 2048;

//+++++++++++++
AbstractMp3Streamer::AbstractMp3Streamer(Audio::Mp3Decoder *decoder)
    :decoder(decoder)
{

}

AbstractMp3Streamer::~AbstractMp3Streamer(){

}

void AbstractMp3Streamer::processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out){
    if(samplesBuffer.empty()){
        return;
    }
    int samplesToRender = std::min(out.getFrameLenght(), (int)samplesBuffer[0].size());
    AudioSamplesBuffer buffer(samplesBuffer.size(), samplesToRender);
    for (int c = 0; c < buffer.getChannels(); ++c) {
        for (int s = 0; s < buffer.getFrameLenght(); ++s) {
            buffer.set(c, s, samplesBuffer[c].front());
            samplesBuffer[c].pop_front();
        }
    }
    out.add(buffer);
}

void AbstractMp3Streamer::decodeBytesFromDevice(QIODevice* device, const unsigned int bytesToRead){
    char inputBuffer[bytesToRead];
    qint64 totalBytesToProcess = device->read(inputBuffer, bytesToRead);

    if(totalBytesToProcess > 0){
        int bytesProcessed = 0;
        char* in = inputBuffer;
        while(bytesProcessed < totalBytesToProcess){//splite bytesReaded in chunks to avoid a very large decoded buffer
            int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed), MAX_BYTES_PER_DECODING);//chunks maxsize is 2048 bytes
            const Audio::AudioSamplesBuffer* decodedBuffer = decoder->decode(in, bytesToProcess);
            //prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            //+++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            QMutexLocker locker(&mutex);
            if(samplesBuffer.empty() && decodedBuffer->getFrameLenght() > 0){
                for (int channel = 0; channel < decodedBuffer->getChannels(); ++channel) {
                    samplesBuffer.push_back(std::deque<float>());
                }
            }

            for(int channel=0; channel < samplesBuffer.size(); ++channel){
                for (int sample = 0; sample < decodedBuffer->getFrameLenght(); ++sample) {
                    samplesBuffer[channel].push_back(decodedBuffer->get(channel, sample));
                }
            }
        }
    }
}

//+++++++++++++++++++++++++++++++++++++++

RoomStreamerNode::RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds)
    : AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
      buffering(true),
      bufferSize(bufferTimeInSeconds * 44100)//TODO melhorar isso
{
    QNetworkReply* reply = httpClient.get(QNetworkRequest(streamPath));
    QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(reply_read()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(reply_error(QNetworkReply::NetworkError)));
    this->device = reply;
}

void RoomStreamerNode::reply_error(QNetworkReply::NetworkError error){
    qDebug() << "ERROR" ;
}

void RoomStreamerNode::reply_read(){
    QMutexLocker(&this->mutex);
    AbstractMp3Streamer::decodeBytesFromDevice(device, device->bytesAvailable());
    if(buffering && !samplesBuffer.empty() && samplesBuffer[0].size() >= bufferSize){
        buffering = false;
    }

}

RoomStreamerNode::~RoomStreamerNode(){
    qDebug() << "RoomStreamerNode destructor!";
}

void RoomStreamerNode::processReplacing(AudioSamplesBuffer & in, AudioSamplesBuffer &out){
    if(buffering){
        return;
    }
    AbstractMp3Streamer::processReplacing(in, out);
}

//++++++++++++++++++

AudioFileStreamerNode::AudioFileStreamerNode(QString file)
    :   AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    QFile* f = new QFile(file);
    if(!f->open(QIODevice::ReadOnly)){
        qDebug() << "não foi possivel abrir o arquivo " << file;
    }
    this->device = f;
}

AudioFileStreamerNode::~AudioFileStreamerNode(){
    //device->deleteLater();
}

void AudioFileStreamerNode::processReplacing(AudioSamplesBuffer & in, AudioSamplesBuffer &out){
    decodeBytesFromDevice(this->device, 1024 + 256);
    AbstractMp3Streamer::processReplacing(in, out);
}
