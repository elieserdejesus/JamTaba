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
      //faderProcessor(0, 1, 44100*3),//3 seconds fade in
      decoder(decoder),
      device(nullptr),
      streaming(false),
      bufferedSamples(2, 4096),
      writeIndex(0), readIndex(0), available(0)
{
    //addProcessor(faderProcessor);
    bufferedSamples.setFrameLenght(0);//reset internal offset
}

AbstractMp3Streamer::~AbstractMp3Streamer(){
	delete decoder;
}

void AbstractMp3Streamer::stopCurrentStream(){
    qCDebug(roomStreamer) << "stopping room stream";

    //QMutexLocker locker(&mutex);
    //faderProcessor.reset();//aply fadein in next stream
    if(device){
        //device->close();
        decoder->reset();//discard unprocessed bytes
        device->deleteLater();
        device = nullptr;
        bufferedSamples.zero();//discard samples
        streaming = false;
    }
    bytesToDecode.clear();
}

int AbstractMp3Streamer::getSamplesToRender(int targetSampleRate, int outLenght){
    bool needResampling =  needResamplingFor(targetSampleRate);
    int samplesToRender = needResampling ? getInputResamplingLength(getSampleRate(), targetSampleRate, outLenght) : outLenght;
    return samplesToRender;
}

void AbstractMp3Streamer::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int targetSampleRate, const Midi::MidiBuffer &midiBuffer){
    Q_UNUSED(in);
    //QMutexLocker locker(&mutex);

    if(bufferedSamples.isEmpty() || !streaming){
        return;
    }

    int outLenght = std::min(out.getFrameLenght(), bufferedSamples.getFrameLenght());
    int samplesToRender = getSamplesToRender(targetSampleRate, outLenght);
    if(samplesToRender <= 0){
        return;
    }

    internalInputBuffer.setFrameLenght(samplesToRender);
    int channels = qMin(internalInputBuffer.getChannels(), bufferedSamples.getChannels());
    for (int s = 0; s < samplesToRender; ++s) {
        for (int c = 0; c < channels; ++c) {
            internalInputBuffer.set(c, s, bufferedSamples.get(c, readIndex));
        }
        readIndex = (readIndex + 1) % bufferedSamples.getFrameLenght();
        available--;
    }
    qWarning() << "read index:" << readIndex << " write index:" << writeIndex << " available" << available;

    //internalInputBuffer.set(bufferedSamples);

    if( needResamplingFor(targetSampleRate)){
        const Audio::SamplesBuffer& resampledBuffer = resampler.resample(internalInputBuffer, outLenght );
        internalOutputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
        internalOutputBuffer.set(resampledBuffer);
    }
    else{
        internalOutputBuffer.setFrameLenght(out.getFrameLenght());
        internalOutputBuffer.set(internalInputBuffer);
    }

    //qWarning() << "samples to render:"<< samplesToRender << " out.frameLenght:" << out.getFrameLenght() << " bufferedSamples:" << bufferedSamples.getFrameLenght();

    //internalOutputBuffer.setFrameLenght(samplesToRender);
    //internalOutputBuffer.zero();
    //internalOutputBuffer.set(bufferedSamples);//copy 'samplesToRender' samples to internalOutputBuffer

    //qCDebug(roomStreamer) << "discarding " << samplesToRender << " samples";
    //bufferedSamples.discardFirstSamples(samplesToRender);//keep non rendered samples for next audio callback
    //qCDebug(roomStreamer) << "new frameLenght:" << bufferedSamples.getFrameLenght();

    if(internalOutputBuffer.getFrameLenght() < out.getFrameLenght()){
        qCDebug(roomStreamer) << out.getFrameLenght() - internalOutputBuffer.getFrameLenght() << " samples missing";
    }

    //faderProcessor.process(internalInputBuffer, internalOutputBuffer, midiBuffer);//aply fade in in stream

    this->lastPeak.update(internalOutputBuffer.computePeak());

    out.add(internalOutputBuffer);
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
            //write in circular buffer scheme
            int channels = qMin(decodedBuffer->getChannels(), bufferedSamples.getChannels());
            for (int s = 0; s < decodedBuffer->getFrameLenght(); ++s) {
                for (int c = 0; c < channels; ++c) {
                    bufferedSamples.set(c, writeIndex, decodedBuffer->get(c, s));
                }
                writeIndex = (writeIndex + 1) % bufferedSamples.getFrameLenght();
                available++;
            }
        }
        qWarning() << "read index:" << readIndex << " write index:" << writeIndex << " available" << available;

        bytesToDecode = bytesToDecode.right(bytesToDecode.size() - bytesProcessed);
        if(bytesToDecode.isEmpty()){
            qCDebug(roomStreamer) << bytesProcessed << " decoded  bytesToDecode: " << bytesToDecode.size();
        }
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
NinjamRoomStreamerNode::NinjamRoomStreamerNode(QUrl streamPath, int bufferTimeInSeconds)
    : AbstractMp3Streamer( new Mp3DecoderMiniMp3()),
      bufferTime(bufferTimeInSeconds),
      httpClient(nullptr)
      //osc(440, 44100)
{

    setStreamPath(streamPath.toString());
    bufferedSamples.setFrameLenght(bufferTimeInSeconds * 2 * 44100);
    writeIndex = bufferedSamples.getFrameLenght()/2;
    available = writeIndex;
}

NinjamRoomStreamerNode::NinjamRoomStreamerNode(int bufferTimeInSeconds)
    :AbstractMp3Streamer( new Mp3DecoderMiniMp3()),
      bufferTime(bufferTimeInSeconds),
      httpClient(nullptr)
      //osc(440, 44100)
{
    setStreamPath("");
    bufferedSamples.setFrameLenght(bufferTimeInSeconds * 2 * 44100);
    writeIndex = bufferedSamples.getFrameLenght()/2;
    available = writeIndex;

    setStreamPath("http://vprclassical.streamguys.net/vprclassical128.mp3");
}


bool NinjamRoomStreamerNode::needResamplingFor(int targetSampleRate) const{
    if(!streaming ){
        return false;
    }
    return  AbstractMp3Streamer::needResamplingFor(targetSampleRate);
}

void NinjamRoomStreamerNode::initialize(QString streamPath){
    AbstractMp3Streamer::initialize(streamPath);
    //buffering =  true;
    bufferedSamples.zero();
    bytesToDecode.clear();
    if(!streamPath.isEmpty()){
        qCDebug(roomStreamer) << "connecting in " << streamPath;
        if(httpClient){
            httpClient->deleteLater();
        }
        httpClient = new QNetworkAccessManager(this);
        QNetworkReply* reply = httpClient->get(QNetworkRequest(QUrl(streamPath)));
        QObject::connect(reply, SIGNAL(readyRead()), this, SLOT(on_reply_read()));
        QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(on_reply_error(QNetworkReply::NetworkError)));
        this->device = reply;
    }
}

void NinjamRoomStreamerNode::on_reply_error(QNetworkReply::NetworkError /*error*/){
    QString msg = "ERROR playing room stream";
    qCCritical(roomStreamer) << msg;
    emit error(msg);
}

void NinjamRoomStreamerNode::on_reply_read(){
    if(!device){
        qCDebug(roomStreamer) << "device is null!";
        return;
    }
    if(device->isOpen() && device->isReadable()){
        QMutexLocker locker(&mutex);
        bytesToDecode.append(device->readAll());
        int toDecode = 0;
        if(available <= 0){
            toDecode = bytesToDecode.size();
        }
        else{
            if(available < bufferedSamples.getFrameLenght() / 4){
                toDecode = 1024;
            }
            else{
                toDecode = 128;
            }
        }
        decode( toDecode );
        qCDebug(roomStreamer) << "bytes downloaded  bytesToDecode:"<<bytesToDecode.size() << " bufferedSamples: " << bufferedSamples.getFrameLenght();
    }
    else{
        qCCritical(roomStreamer) << "problem in device!";
    }
}

NinjamRoomStreamerNode::~NinjamRoomStreamerNode(){
    qCDebug(roomStreamer) << "RoomStreamerNode destructor!";
}

void NinjamRoomStreamerNode::processReplacing(const SamplesBuffer & in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer &midiBuffer){
    Q_UNUSED(in)
    QMutexLocker locker(&mutex);
    //long long start = QDateTime::currentMSecsSinceEpoch();
    AbstractMp3Streamer::processReplacing(in, out, sampleRate, midiBuffer);
    //qWarning() << "available:" << available;
//    long long time = QDateTime::currentMSecsSinceEpoch() - start;
//    if(time > 1){
//        qWarning() << "AbstractMp3Streamer::processReplacing: " << time << "ms";
//    }
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

TestStreamerNode::TestStreamerNode(int sampleRate)
    :AbstractMp3Streamer(new Mp3DecoderMiniMp3())
{
    oscilator = new OscillatorAudioNode(440, sampleRate);
    playing = true;
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
        //faderProcessor.process(in, out, midiBuffer);
    }
    lastPeak.update( out.computePeak());
}


void TestStreamerNode::setStreamPath(QString streamPath){
    Q_UNUSED(streamPath);
    //faderProcessor.reset();
    playing = true;
    bytesToDecode.clear();
}

void TestStreamerNode::stopCurrentStream(){
    playing = false;
}

