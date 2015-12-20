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
    //qWarning() << "Destruindo ninjamTrack Node " << this->getID();
}

void NinjamTrackNode::discardIntervals(){
    QMutexLocker locker(&mutex);
    intervals.clear();
    qDebug() << "intervals discarded";
}

bool NinjamTrackNode::startNewInterval(){
    QMutexLocker locker(&mutex);

    if(!intervals.isEmpty()){
        decoder.setInput(intervals.front());
        intervals.removeFirst();
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
//    if(!isActivated()){
//        return;
//    }
    intervals.append(vorbisData);//enqueue a new interval
}
//++++++++++++++++++++++++++++++++++++++


int NinjamTrackNode::getFramesToProcess(int targetSampleRate, int outFrameLenght){
    return needResamplingFor(targetSampleRate) ? getInputResamplingLength(getSampleRate(), targetSampleRate, outFrameLenght) : outFrameLenght;
}

void NinjamTrackNode::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer){

    if(!playing){
        return;
    }

    //qint64 inicio = QDateTime::currentMSecsSinceEpoch();

    int totalDecoded = 0;
    int framesToDecode = getFramesToProcess(sampleRate, out.getFrameLenght()) ;
    internalInputBuffer.setFrameLenght(framesToDecode);
    internalInputBuffer.zero();
    while(totalDecoded < framesToDecode ){
        const Audio::SamplesBuffer& decodedBuffer = decoder.decode(framesToDecode - totalDecoded);
        if(decodedBuffer.getFrameLenght() > 0){
            internalInputBuffer.add(decodedBuffer, totalDecoded);//total decoded is the offset
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

        internalInputBuffer.setFrameLenght(totalDecoded);

        if(needResamplingFor(sampleRate)){
            const Audio::SamplesBuffer& resampledBuffer = resampler.resample(internalInputBuffer, out.getFrameLenght() );
            internalInputBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
            internalInputBuffer.set(resampledBuffer);
            if(internalInputBuffer.getFrameLenght() != out.getFrameLenght()){
                qWarning() << internalInputBuffer.getFrameLenght() << " != " << out.getFrameLenght();
            }
        }
        Audio::AudioNode::processReplacing(in, out, sampleRate, midiBuffer);//process internal buffer pan, gain, etc
    }
    //qint64 processamento = QDateTime::currentMSecsSinceEpoch() - inicio;

}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const{
    if(playing && decoder.isInitialized()){
        return decoder.getSampleRate() != targetSampleRate;
    }
    return false;
}
