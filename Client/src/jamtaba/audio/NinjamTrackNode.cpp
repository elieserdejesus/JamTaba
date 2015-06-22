#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
//#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>


NinjamTrackNode::NinjamTrackNode(int ID)
    :playing(false), ID(ID)
{

}

NinjamTrackNode::~NinjamTrackNode()
{
    qDebug() << "Destruindo ninjamTrack Node " << this->getID();
}

void NinjamTrackNode::startNewInterval(){
    QMutexLocker locker(&mutex);
    if(!isActivated()){
        return;
    }
    if(!intervals.isEmpty()){
        decoder.setInput(intervals.front());
        intervals.removeFirst();
        decoder.reset();//head the headers
        playing = true;
    }
    else{
        playing = false;
    }
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
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->zero();
    while(totalDecoded < out.getFrameLenght() ){
        const Audio::SamplesBuffer* decodedBuffer = decoder.decode(out.getFrameLenght() - totalDecoded);
        if(decodedBuffer->getFrameLenght() > 0){
            out.add(*decodedBuffer, totalDecoded);//total decoded is the offset
        }
        totalDecoded += decodedBuffer->getFrameLenght();
        if(decodedBuffer->getFrameLenght() == 0){
            break;
        }
    }

    if(totalDecoded > 0){
        Audio::AudioNode::processReplacing(in, out);
    }

}
