#include "NinjamTrackNode.h"
#include "../audio/core/AudioDriver.h"
#include "streambuffer.h"
#include <QDataStream>
#include <QDebug>

NinjamTrackNode::NinjamTrackNode()
    :decoder(VorbisDecoder(encodedBytes)), streamBuffer(nullptr)
{

}

NinjamTrackNode::~NinjamTrackNode()
{
    if(streamBuffer){
        delete streamBuffer;
    }
}

void NinjamTrackNode::addEncodedBytes(QByteArray bytes){
    encodedBytes.append(bytes);
    if(!decoder.isInitialized()){
        decoder.initialize();//parse ogg header to get channels and sample rate
        int streamBufferCapacity = decoder.getSampleRate() * 2;//2 seconds of buffered samples
        streamBuffer = new Audio::StreamBuffer(decoder.getChannels(), streamBufferCapacity);
    }
}
//++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++
void NinjamTrackNode::copyMonoDecodedBytesToStream(int decodedBytes){
    if(decoder.isStereo()){
        qCritical() << "decoder is stereo!";
    }
    QByteArray byteArray(outBuffer, decodedBytes/2);
    QDataStream stream(&byteArray, QIODevice::ReadOnly);
    int framesDecoded = decodedBytes/2/decoder.getChannels();//2 bytes per sample
    qint16 sampleValue;
    for (int s = 0; s < framesDecoded; ++s) {
        stream >> sampleValue;
        streamBuffer->add(sampleValue / 32767.0);
    }
}

void NinjamTrackNode::copyStereoDecodedBytesToStream(int decodedBytes){
    if(decoder.isMono()){
        qCritical() << "decoder is mono!";
    }
    QByteArray leftChannel(outBuffer, decodedBytes/2);
    QByteArray rightChannel(outBuffer + decodedBytes/2, decodedBytes/2);
    QDataStream leftStream(&leftChannel, QIODevice::ReadOnly);
    QDataStream rightStream(&rightChannel, QIODevice::ReadOnly);

    int framesDecoded = decodedBytes/2/decoder.getChannels();//2 bytes per sample
    qint16 sampleValue;
    for (int s = 0; s < framesDecoded; ++s) {
        leftStream >> sampleValue;//left channel stream
        float leftSample = sampleValue / 32767.0;
        rightStream >> sampleValue;//right channel stream
        float rightSample = sampleValue / 32767.0;
        streamBuffer->add(leftSample, rightSample);
    }
}

void NinjamTrackNode::processReplacing(Audio::SamplesBuffer &in, Audio::SamplesBuffer &out){
    if(!streamBuffer){
        return;
    }
    //faz o decoding para o stream buffer, assim caso sejam decodificadas mais amostras que o necessário neste callbackd e áudio
    //as amostras excedentes serão utilizadas no próximo callback
    while(decoder.hasMoreSamplesToDecode() && streamBuffer->getAvailable() < out.getFrameLenght()){
        int bytesDecoded = decoder.decode(outBuffer);
        if(bytesDecoded > 0){
            if(decoder.isStereo()){
                copyStereoDecodedBytesToStream(bytesDecoded);
            }
            else{
                copyMonoDecodedBytesToStream(bytesDecoded);
            }
        }
        qDebug() << "bytes decoded: " << bytesDecoded;
    }

    //copy samples from streamBuffer to internal buffer, and the super class process the internalBuffer with pan,gain, peaks, etc.
    internalBuffer->setFrameLenght(std::min(streamBuffer->getAvailable(), out.getFrameLenght()));
    internalBuffer->zero();
    streamBuffer->fillBuffer(*internalBuffer);

    Audio::AudioNode::processReplacing(in, out);
}
