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

Q_LOGGING_CATEGORY(roomStreamer, "roomStreamer")

using namespace Audio;

const int AbstractMp3Streamer::MAX_BYTES_PER_DECODING = 2048;

//+++++++++++++
AbstractMp3Streamer::AbstractMp3Streamer( Audio::Mp3Decoder *decoder)
    :
      faderProcessor(0, 1, 44100*3),//3 seconds fade in
      decoder(decoder),
      device(nullptr),
      streaming(false),
      bufferedSamples(2, 4096)
{
    //addProcessor(faderProcessor);
    bufferedSamples.setFrameLenght(0);//reset internal offset
}

AbstractMp3Streamer::~AbstractMp3Streamer(){

}

void AbstractMp3Streamer::stopCurrentStream(){
    qCDebug(roomStreamer) << "stopping room stream";

    //QMutexLocker locker(&mutex);
    faderProcessor.reset();//aply fadein in next stream
    if(device){
        device->close();
        decoder->reset();//discard unprocessed bytes
        device = nullptr;
        bufferedSamples.zero();//discard samples
        streaming = false;
    }
    bytesToDecode.clear();
}

void AbstractMp3Streamer::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer &midiBuffer){
    Q_UNUSED(in);
    Q_UNUSED(sampleRate);
    //QMutexLocker locker(&mutex);
    if(bufferedSamples.isEmpty() || !streaming){
        //qCDebug(roomStreamer) << "buffered samples is empty";
        return;
    }
    int samplesToRender = std::min(out.getFrameLenght(), bufferedSamples.getFrameLenght());
    //qCDebug(roomStreamer) << "samples to render:"<< samplesToRender << " out.frameLenght:" << out.getFrameLenght() << " bufferedSamples:" << bufferedSamples.getFrameLenght();

    internalBuffer.setFrameLenght(samplesToRender);
    internalBuffer.zero();
    internalBuffer.set(bufferedSamples);

    //qCDebug(roomStreamer) << "discarding " << samplesToRender << " samples";
    bufferedSamples.discardFirstSamples(samplesToRender);//keep non rendered samples for next audio callback
    //qCDebug(roomStreamer) << "new frameLenght:" << bufferedSamples.getFrameLenght();

    if(internalBuffer.getFrameLenght() < out.getFrameLenght()){
        qCDebug(roomStreamer) << out.getFrameLenght() - internalBuffer.getFrameLenght() << " samples missing";
    }

    this->lastPeak.update(internalBuffer.computePeak());

    faderProcessor.process(internalBuffer, midiBuffer);//aply fade in in stream

    out.add(internalBuffer);
}

void AbstractMp3Streamer::initialize(QString streamPath){
    //Q_UNUSED(streamPath);
    streaming = !streamPath.isNull() && !streamPath.isEmpty();
}

int AbstractMp3Streamer::getSampleRate() const{
    return decoder->getSampleRate();
}

bool AbstractMp3Streamer::needResamplingFor(int targetSampleRate) const{
    if(!streaming ){
        return false;
    }
    return targetSampleRate != getSampleRate();
}

void AbstractMp3Streamer::decode(const unsigned int maxBytesToDecode){
    QMutexLocker locker(&mutex);
    if(!device ){
        return;
    }

    qint64 totalBytesToProcess = std::min((int)maxBytesToDecode, bytesToDecode.size());
    char* in = bytesToDecode.data();

    if(totalBytesToProcess > 0){
        int bytesProcessed = 0;
        while(bytesProcessed < totalBytesToProcess){//split bytesReaded in chunks to avoid a very large decoded buffer
            int bytesToProcess = std::min((int)(totalBytesToProcess - bytesProcessed), MAX_BYTES_PER_DECODING);//chunks maxsize is 2048 bytes
            const Audio::SamplesBuffer* decodedBuffer = decoder->decode(in, bytesToProcess);
            //prepare in for the next decoding
            in += bytesToProcess;
            bytesProcessed += bytesToProcess;
            //+++++++++++++++++  PROCESS DECODED SAMPLES ++++++++++++++++
            bufferedSamples.append(*decodedBuffer);
        }
        bytesToDecode = bytesToDecode.right(bytesToDecode.size() - bytesProcessed);
    }
}

void AbstractMp3Streamer::setStreamPath(QString streamPath){
    stopCurrentStream();
    initialize(streamPath);
}

//+++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++
RoomStreamerNode::RoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds)
    : AbstractMp3Streamer( new Mp3DecoderMiniMp3()),
      bufferSize(bufferTimeInSeconds * 48000)
{
    setStreamPath(streamPath.toString());
}

RoomStreamerNode::RoomStreamerNode(int bufferTimeInSeconds)
    :AbstractMp3Streamer( new Mp3DecoderMiniMp3()),
      bufferSize(bufferTimeInSeconds * 48000)//TODO melhorar isso
{
    setStreamPath("");
}


bool RoomStreamerNode::needResamplingFor(int targetSampleRate) const{
    if(!streaming || buffering){
        return false;
    }
    return AbstractMp3Streamer::needResamplingFor(targetSampleRate);
}

void RoomStreamerNode::initialize(QString streamPath){
    AbstractMp3Streamer::initialize(streamPath);
    buffering = true;
    bufferedSamples.zero();
    bytesToDecode.clear();
    if(!streamPath.isEmpty()){
        QNetworkReply* reply = httpClient.get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(reply_read()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(reply_error(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}


void RoomStreamerNode::reply_error(QNetworkReply::NetworkError /*error*/){
    qCCritical(roomStreamer) << "ERROR playing room stream" ;
}

void RoomStreamerNode::reply_read(){
    if(!device){
        return;
    }
    QMutexLocker locker(&mutex);
    bytesToDecode.append(device->readAll());

}

RoomStreamerNode::~RoomStreamerNode(){
    //qDebug() << "RoomStreamerNode destructor!";
}

void RoomStreamerNode::processReplacing(const SamplesBuffer & in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer &midiBuffer){
    //QMutexLocker locker(&mutex);
    if(buffering || bufferedSamples.getFrameLenght() < bufferSize){
        decode(1024);
    }
    if(buffering && bufferedSamples.getFrameLenght() >= bufferSize){
        buffering = false;
    }

    if(!buffering){
        AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
    }
    else{//buffering
        lastPeak.zero();
    }

}

//++++++++++++++++++

AudioFileStreamerNode::AudioFileStreamerNode(QString file)
    :   AbstractMp3Streamer( new Mp3DecoderMiniMp3())
{
    setStreamPath(file);
}

void AudioFileStreamerNode::initialize(QString streamPath){
    AbstractMp3Streamer::initialize(streamPath);
    QFile* f = new QFile(streamPath);
    if(!f->open(QIODevice::ReadOnly)){
        qCCritical(roomStreamer) << "não foi possivel abrir o arquivo " << streamPath;
    }
    this->device = f;
    bytesToDecode.append(f->readAll());
}

AudioFileStreamerNode::~AudioFileStreamerNode(){
    //device->deleteLater();
}

void AudioFileStreamerNode::processReplacing(const SamplesBuffer & in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer &midiBuffer){
    while(bufferedSamples.getFrameLenght() < out.getFrameLenght()){
        //bytesToDecode.append(device->read(1024 + 256));
        decode(1024 + 1024);
    }


    AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/*
/*
TestStreamerNode::TestStreamerNode(int sampleRate)
    :AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    oscilator = new OscillatorAudioNode(2, sampleRate);
    playing = false;
}

TestStreamerNode::~TestStreamerNode(){
    delete oscilator;
}

void TestStreamerNode::initialize(QString streamPath){
    AbstractMp3Streamer::initialize(streamPath);
}

void TestStreamerNode::processReplacing(const Audio::SamplesBuffer & in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer &midiBuffer){

    if(playing){
        oscilator->processReplacing(in, out, sampleRate, midiBuffer);
        faderProcessor.process(out, midiBuffer);
    }
    lastPeak.update( out.computePeak());
}


void TestStreamerNode::setStreamPath(QString streamPath){
    faderProcessor.reset();
    playing = true;
    bytesToDecode.clear();
}

void TestStreamerNode::stopCurrentStream(){
    playing = false;
}
*/
