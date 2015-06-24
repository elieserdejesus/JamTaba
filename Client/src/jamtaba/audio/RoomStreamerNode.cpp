#include "RoomStreamerNode.h"
#include "codec.h"
#include "core/AudioDriver.h"
#include "core/SamplesBuffer.h"
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
#include <QMutexLocker>
#include <QFile>

using namespace Audio;

const int AbstractMp3Streamer::MAX_BYTES_PER_DECODING = 2048;

//+++++++++++++
AbstractMp3Streamer::AbstractMp3Streamer(Audio::Mp3Decoder *decoder)
    :
      faderProcessor(FaderProcessor(0, 1, 44100*3)),//3 seconds fade in
      decoder(decoder),
      device(nullptr)
{
    //addProcessor(faderProcessor);
}

AbstractMp3Streamer::~AbstractMp3Streamer(){

}

void AbstractMp3Streamer::stopCurrentStream(){
    QMutexLocker locker(&mutex);
    faderProcessor.reset();//aply fadein in next stream
    if(device){
        device->close();
        decoder->reset();//discard unprocessed bytes
        device = nullptr;
        samplesBuffer.clear();//discard samples
    }
}

void AbstractMp3Streamer::processReplacing(SamplesBuffer &/*in*/, SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    if(samplesBuffer.empty()){
        return;
    }
    int samplesToRender = std::min(out.getFrameLenght(), (int)samplesBuffer[0].size());
    SamplesBuffer buffer(samplesBuffer.size(), samplesToRender);
    for (int c = 0; c < buffer.getChannels(); ++c) {
        for (int s = 0; s < buffer.getFrameLenght(); ++s) {
            buffer.set(c, s, samplesBuffer[c].front());
            samplesBuffer[c].pop_front();
        }
    }
    this->lastPeak.update(buffer.computePeak());

    faderProcessor.process(buffer);//aply fade in in stream

    out.add(buffer);
}

void AbstractMp3Streamer::decodeBytesFromDevice(QIODevice* device, const unsigned int bytesToRead){
    QMutexLocker locker(&mutex);
    if(!device){
        return;
    }
    char* inputBuffer = new char[bytesToRead];
    qint64 totalBytesToProcess = device->read(inputBuffer, bytesToRead);

    if(totalBytesToProcess > 0){
        int bytesProcessed = 0;
        char* in = inputBuffer;
        while(bytesProcessed < totalBytesToProcess){//splite bytesReaded in chunks to avoid a very large decoded buffer
            int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed), MAX_BYTES_PER_DECODING);//chunks maxsize is 2048 bytes
            const Audio::SamplesBuffer* decodedBuffer = decoder->decode(in, bytesToProcess);
            //prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            //+++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            if(samplesBuffer.empty() && decodedBuffer->getFrameLenght() > 0){
                qDebug() << "canais:"<<decodedBuffer->getChannels();
                for (int channel = 0; channel < decodedBuffer->getChannels(); ++channel) {
                    samplesBuffer.push_back(std::deque<float>());
                }
            }

            for(unsigned int channel=0; channel < samplesBuffer.size(); ++channel){
                for (int sample = 0; sample < decodedBuffer->getFrameLenght(); ++sample) {
                    samplesBuffer[channel].push_back(decodedBuffer->get(channel, sample));
                }
            }
        }
    }
    delete inputBuffer;
}

void AbstractMp3Streamer::setStreamPath(QString streamPath){
    stopCurrentStream();
    initialize(streamPath);
}

//+++++++++++++++++++++++++++++++++++++++
RoomStreamerNode::RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds)
    : AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
      bufferSize(bufferTimeInSeconds * 44100)//TODO melhorar isso
{
    setStreamPath(streamPath.toString());
}

RoomStreamerNode::RoomStreamerNode(int bufferTimeInSeconds)
    :AbstractMp3Streamer(new Mp3DecoderMiniMp3()),
      bufferSize(bufferTimeInSeconds * 44100)//TODO melhorar isso
{
    setStreamPath("");
}


void RoomStreamerNode::initialize(QString streamPath){
    buffering = true;
    if(!streamPath.isEmpty()){
        QNetworkReply* reply = httpClient.get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(reply_read()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(reply_error(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}


void RoomStreamerNode::reply_error(QNetworkReply::NetworkError /*error*/){
    qDebug() << "ERROR" ;
}

void RoomStreamerNode::reply_read(){
    if(!device){
        return;
    }
    QMutexLocker(&this->mutex);
    AbstractMp3Streamer::decodeBytesFromDevice(device, device->bytesAvailable());
    if(buffering && !samplesBuffer.empty() && (int)samplesBuffer[0].size() >= bufferSize){
        buffering = false;
    }

}

RoomStreamerNode::~RoomStreamerNode(){
    //qDebug() << "RoomStreamerNode destructor!";
}

void RoomStreamerNode::processReplacing(SamplesBuffer & in, SamplesBuffer &out){
    if(buffering){
        lastPeak.zero();
        return;
    }
    AbstractMp3Streamer::processReplacing(in, out);
}

//++++++++++++++++++

AudioFileStreamerNode::AudioFileStreamerNode(QString file)
    :   AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    setStreamPath(file);
}

void AudioFileStreamerNode::initialize(QString streamPath){
    QFile* f = new QFile(streamPath);
    if(!f->open(QIODevice::ReadOnly)){
        qDebug() << "não foi possivel abrir o arquivo " << streamPath;
    }
    this->device = f;
}

AudioFileStreamerNode::~AudioFileStreamerNode(){
    //device->deleteLater();
}

void AudioFileStreamerNode::processReplacing(SamplesBuffer & in, SamplesBuffer &out){
    decodeBytesFromDevice(this->device, 1024 + 256);
    AbstractMp3Streamer::processReplacing(in, out);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/*

TestStreamerNode::TestStreamerNode()
    :AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    oscilator = new OscillatorAudioNode(2, 44100);
    playing = false;
}

TestStreamerNode::~TestStreamerNode(){
    delete oscilator;
}

void TestStreamerNode::initialize(QString /*streamPath*/){

}

void TestStreamerNode::processReplacing(SamplesBuffer & in, SamplesBuffer &out){
    if(playing){
        oscilator->processReplacing(in, out);
        faderProcessor.process(out);
    }
    lastPeak.update( out.computePeak());
}


void TestStreamerNode::setStreamPath(QString /*streamPath*/){
    faderProcessor.reset();
    playing = true;
}

void TestStreamerNode::stopCurrentStream(){
    playing = false;
}
