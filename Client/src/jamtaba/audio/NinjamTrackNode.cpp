#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
//#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>
#include <QDateTime>


NinjamTrackNode::NinjamTrackNode(int ID)
    : playing(false), ID(ID), processingLastPartOfInterval(false)
      //resamplerLeft(Resampler())
{

}

int NinjamTrackNode::getSampleRate() const{
    return decoder.getSampleRate();
}

NinjamTrackNode::~NinjamTrackNode()
{
    //QMutexLocker locker(&mutex);
    //qDebug() << "Destruindo ninjamTrack Node " << this->getID();
}

void NinjamTrackNode::discardIntervals(){
    QMutexLocker locker(&mutex);
    intervals.clear();
    qDebug() << "intervals discarded";
}

bool NinjamTrackNode::startNewInterval(){
    QMutexLocker locker(&mutex);
    if(!isActivated()){
        return false;
    }
    if(!intervals.isEmpty()){
        decoder.setInput(intervals.front());
        intervals.removeFirst();
        //qDebug() << "total samples decoded: " << decoder.getTotalDecodedSamples();
        decoder.reset();//head the headers from new interval
        playing = true;
    }
    else{
        playing = false;
    }
    return playing;
}

void NinjamTrackNode::addVorbisEncodedInterval(QByteArray vorbisData){
    QMutexLocker locker(&mutex);
    if(!isActivated()){
        return;
    }
    intervals.append(vorbisData);//enqueue a new interval
}
//++++++++++++++++++++++++++++++++++++++


int NinjamTrackNode::getFramesToProcess(int targetSampleRate, int outFrameLenght){
    return needResamplingFor(targetSampleRate) ? getInputResamplingLength(getSampleRate(), targetSampleRate, outFrameLenght) : outFrameLenght;
}

void NinjamTrackNode::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate){
    {
        QMutexLocker locker(&mutex);
        if(!isActivated()){
            return;
        }
    }
    if(!playing){
        return;
    }

    //qint64 inicio = QDateTime::currentMSecsSinceEpoch();

    int totalDecoded = 0;
    int framesToDecode = getFramesToProcess(sampleRate, out.getFrameLenght()) ;
    internalBuffer.setFrameLenght(framesToDecode);
    internalBuffer.zero();
    while(totalDecoded < framesToDecode ){
        const Audio::SamplesBuffer& decodedBuffer = decoder.decode(framesToDecode - totalDecoded);
        if(decodedBuffer.getFrameLenght() > 0){
            internalBuffer.add(decodedBuffer, totalDecoded);//total decoded is the offset
            totalDecoded += decodedBuffer.getFrameLenght();
        }
        else{//no more samples to decode
            break;
        }
    }
//    if(totalDecoded != framesToDecode){
//        qWarning() << "totalDecoded != framesToDecode" << totalDecoded << " != " << framesToDecode;
//    }

//    if(totalDecoded == 0 && !processingLastPartOfInterval){
//        qWarning() << "totalDecoded ZERO e nao estah processando ultima parte do intervalo";
//    }

    if(totalDecoded > 0){

        internalBuffer.setFrameLenght(totalDecoded);

        if(needResamplingFor(sampleRate)){
            const Audio::SamplesBuffer& resampledBuffer = resampler.resample(internalBuffer, out.getFrameLenght() );
            internalBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalBuffer.set(resampledBuffer);
            if(internalBuffer.getFrameLenght() != out.getFrameLenght()){
                qWarning() << internalBuffer.getFrameLenght() << " != " << out.getFrameLenght();
            }
        }
        Audio::AudioNode::processReplacing(in, out, sampleRate);//process internal buffer pan, gain, etc
    }
    //qint64 processamento = QDateTime::currentMSecsSinceEpoch() - inicio;

}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const{
    if(playing && decoder.isInitialized()){
        return decoder.getSampleRate() != targetSampleRate;
    }
    return false;
}
