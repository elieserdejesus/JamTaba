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
    qDebug() << "Destruindo ninjamTrack Node " << this->getID();
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

void NinjamTrackNode::processReplacing(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
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
    internalBuffer.setFrameLenght(out.getFrameLenght());
    internalBuffer.zero();
    while(totalDecoded < out.getFrameLenght() ){
        const Audio::SamplesBuffer& decodedBuffer = decoder.decode(out.getFrameLenght() - totalDecoded);
        if(decodedBuffer.getFrameLenght() > 0){
            internalBuffer.add(decodedBuffer, totalDecoded);//total decoded is the offset
            totalDecoded += decodedBuffer.getFrameLenght();
        }
        else{//no more samples to decode
            break;
        }
    }

    if(totalDecoded > 0){
        Audio::AudioNode::processReplacing(in, out);//process internal buffer pan, gain, etc
    }
}

bool NinjamTrackNode::needResamplingFor(int targetSampleRate) const{
    if(playing){
        return decoder.getSampleRate() != targetSampleRate;
    }
    return false;
}
