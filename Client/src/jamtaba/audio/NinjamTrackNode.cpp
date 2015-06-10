#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>

NinjamTrackNode::NinjamTrackNode()
    : readIndex(0), writeIndex(0), playing(false)
{

}

NinjamTrackNode::~NinjamTrackNode()
{
}

void NinjamTrackNode::startNewInterval(){
    playing = decoders[readIndex].canDecode();
    qDebug() << "playing: " << playing << " readIndex: " << readIndex;
}

void NinjamTrackNode::addEncodedBytes(QByteArray vorbisData, bool lastPartOfInterval){
    decoders[writeIndex].addVorbisData(vorbisData, lastPartOfInterval);
    if(lastPartOfInterval){
        //decoders[writeIndex].initialize();
        writeIndex = (writeIndex + 1) % 2;//use the next
        //decoders[writeIndex].reset();
    }
}
//++++++++++++++++++++++++++++++++++++++

void NinjamTrackNode::processReplacing(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    if(!playing){
        return;
    }
    int totalDecoded = 0;
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->zero();
    if(decoders[readIndex].canDecode()){
        while(totalDecoded < out.getFrameLenght() && decoders[readIndex].hasMoreSamplesToDecode()){
            const Audio::SamplesBuffer* decodedBuffer = decoders[readIndex].decode(out.getFrameLenght() - totalDecoded);
            if(decodedBuffer->getFrameLenght() > 0){
                out.add(*decodedBuffer, totalDecoded);//total decoded is the offset
            }
            totalDecoded += decodedBuffer->getFrameLenght();
            if(decodedBuffer->getFrameLenght() == 0){
                break;
            }
        }
    }
    if(totalDecoded > 0){
        Audio::AudioNode::processReplacing(in, out);
    }
    if(decoders[readIndex].isInitialized() && !decoders[readIndex].hasMoreSamplesToDecode()){//all samples consumed
        decoders[readIndex].reset();
        readIndex = (readIndex + 1) % 2;
    }
}
