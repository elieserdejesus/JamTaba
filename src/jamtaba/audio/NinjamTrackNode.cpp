#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>

NinjamTrackNode::NinjamTrackNode()
    :decoder(VorbisDecoder(encodedBytes))//, streamBuffer(nullptr)
{

}

NinjamTrackNode::~NinjamTrackNode()
{
//    if(streamBuffer){
//        delete streamBuffer;
//    }
}

void NinjamTrackNode::addEncodedBytes(QByteArray bytes){
    encodedBytes.append(bytes);
    if(!decoder.isInitialized()){
        decoder.initialize();//parse ogg header to get channels and sample rate
        decoderOutBuffer = new float*[decoder.getChannels()];
        for (int c = 0; c < decoder.getChannels(); ++c) {
            decoderOutBuffer[c] = new float[4096];
        }
    }
}
//++++++++++++++++++++++++++++++++++++++

void NinjamTrackNode::processReplacing(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    int totalDecoded = 0;
    int internalOffset = 0;
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->zero();
    while(totalDecoded < out.getFrameLenght() && decoder.hasMoreSamplesToDecode()){
        int decoded = decoder.decode(&decoderOutBuffer, out.getFrameLenght() - totalDecoded);
        if(decoded > 0){
            if(decoder.isStereo()){
                for (int s = 0; s < decoded; ++s) {
                    internalBuffer->set(0, internalOffset, decoderOutBuffer[0][s]);//left sample
                    internalBuffer->set(1, internalOffset, decoderOutBuffer[1][s]);//right sample
                    internalOffset++;
                }
            }
            else{
                for (int s = 0; s < decoded; ++s) {
                    internalBuffer->set(0, internalOffset, decoderOutBuffer[0][s]);//mono sample
                    internalOffset++;
                }
            }
        }
        totalDecoded += decoded;
    }
    if(totalDecoded > 0){
        Audio::AudioNode::processReplacing(in, out);
    }

}
