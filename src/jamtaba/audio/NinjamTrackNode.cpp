#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
//#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>


NinjamTrackNode::NinjamTrackNode(int ID)
    : playing(false), ID(ID)
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
    int totalDecoded = 0;


    int framesToDecode= getFramesToProcess(sampleRate, out.getFrameLenght());
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

    //if(totalDecoded > 0){
        if(needResamplingFor(sampleRate)){
              const Audio::SamplesBuffer& resampledBuffer = resampler.resample(internalBuffer, getSampleRate(), false, out.getFrameLenght(), sampleRate );
              internalBuffer.setFrameLenght(resampledBuffer.getFrameLenght());
              internalBuffer.set(resampledBuffer);
        }
        Audio::AudioNode::processReplacing(in, out, sampleRate);//process internal buffer pan, gain, etc
    //}
}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const{
    if(playing){
        return decoder.getSampleRate() != targetSampleRate;
    }
    return false;
}
