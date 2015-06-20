#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
//#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>
#include <QList>
#include <QByteArray>
#include <QMutexLocker>

class NinjamInterval{
public:
    NinjamInterval()
        :downloaded(false){
    }

    void addBytes(QByteArray vorbisData){
        bytes.append(vorbisData);
    }

    QByteArray getBytes() const{
        return bytes;
    }

    void markAsCompleted(){
        this->downloaded = true;
    }

    inline bool isDownloaded() const{return downloaded;}

private:
    bool downloaded;
    QByteArray bytes;
};

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
    if(!intervals.isEmpty() && intervals.front().isDownloaded() ){
        decoder.setInput(intervals.front().getBytes());
        intervals.removeFirst();
        decoder.reset();//head the headers
        playing = true;
    }
    else{
        playing = false;
    }
}

void NinjamTrackNode::addEncodedBytes(QByteArray vorbisData, bool lastPartOfInterval){
    QMutexLocker locker(&mutex);
    if(!isActivated()){
        return;
    }

    if(intervals.isEmpty()){
        intervals.append(NinjamInterval());
    }
    intervals.back().addBytes(vorbisData);
    if(lastPartOfInterval){
        intervals.back().markAsCompleted();//mark interval as completed
        intervals.append(NinjamInterval());//create a new interval to the next interval stream
    }

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
