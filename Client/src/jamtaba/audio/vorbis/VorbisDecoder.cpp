#include "VorbisDecoder.h"
#include <stdexcept>
#include <cstring>
#include <QByteArray>
#include <cmath>
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include <vorbis/vorbisfile.h>
//+++++++++++++++++++++++++++++++++++++++++++
VorbisDecoder::VorbisDecoder()
    : internalBuffer(nullptr),
      initialized(false),
      finished(false),
      vorbisInput(QByteArray()),
      lastPartAdded(false)
{

}
//+++++++++++++++++++++++++++++++++++++++++++
VorbisDecoder::~VorbisDecoder(){
    if(isInitialized()){
        ov_clear(&vorbisFile);
        if(internalBuffer){
            delete internalBuffer;
        }
    }
}
//+++++++++++++++++++++++++++++++++++++++++++
//vorbisfile read callback
size_t VorbisDecoder::readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *datasource){
    QByteArray* byteArray = reinterpret_cast<QByteArray*>(datasource);
    if(byteArray->isEmpty()){
        return 0;//end of stream
    }
    size_t len = std::min( (int)(size * nmemb), byteArray->size());
    memcpy(oggOutBuffer, byteArray->data(), len);
    byteArray->remove(0, len);
    return len;
}
//+++++++++++++++++++++++++++++++++++++++++++
//return decoded samples
const Audio::SamplesBuffer* VorbisDecoder::decode(int maxSamplesToDecode){
    if(!lastPartAdded){//waiti to complete the download of encoded interval
        return &(Audio::SamplesBuffer::ZERO_BUFFER);
    }
    if (!isInitialized()){
        if(!initialize()){
            return &(Audio::SamplesBuffer::ZERO_BUFFER);
        }
    }
    static int currentSection;
    long samplesDecoded = ov_read_float(&vorbisFile, &internalBuffer->samples, maxSamplesToDecode, &currentSection);//currentSection is not used
    if(samplesDecoded < 0){//error
        QString message;
        switch (samplesDecoded) {
            case OV_HOLE: message = "VORBIS ERROR: there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)";
                break;
            case OV_EBADLINK: message = "VORBIS ERROR: an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.";
                break;
            case OV_EINVAL: message = "VORBIS ERROR: the initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.";
        }
        qWarning() << message;
        return &Audio::SamplesBuffer::ZERO_BUFFER;
    }
    internalBuffer->setFrameLenght(samplesDecoded);
    if(samplesDecoded == 0){//sometimes decoder can't decode samples because don't have enough bytes to decode
        finished = true;
        qDebug() << "Decoder finalizado!";
    }

    return internalBuffer;
}
//+++++++++++++++++++++++++++++++++++++++++++
void VorbisDecoder::reset(){
    if(isInitialized()){
        finished = initialized = lastPartAdded = false;
        ov_clear(&vorbisFile);
        qDebug() << "resetou decoder" ;
    }
}

void VorbisDecoder::addVorbisData(QByteArray vorbisData, bool isLastPart){
    if(!finished){
        vorbisInput.append(vorbisData);
    }
    if(isLastPart ){
        lastPartAdded = true;
        //qDebug() << "adicionou a última parte!";
        initialize();
    }
    //qDebug() << "já baixou " << vorbisInput.size() << " bytes";
}

//+++++++++++++++++++++++++++++++++++++++++++
bool VorbisDecoder::initialize(){
    if (!isInitialized()){
        ov_callbacks callbacks;
        callbacks.read_func = readOgg;
        callbacks.seek_func = NULL;
        callbacks.close_func = NULL;
        callbacks.tell_func = NULL;
        int result = ov_open_callbacks((void*)&vorbisInput, &vorbisFile, NULL, 0, callbacks );
        if(result == 0){
            initialized = true;
            finished = false;
            //lastPartAdded = false;
            if(internalBuffer){
                if(internalBuffer->getChannels() != vorbisFile.vi->channels){
                    delete internalBuffer;
                    internalBuffer = new Audio::SamplesBuffer(vorbisFile.vi->channels, 4096);
                }
            }
            else{
                internalBuffer = new Audio::SamplesBuffer(vorbisFile.vi->channels, 4096);
            }
            qDebug() << "inicializou decoder";
        }
        else{
            initialized = false;
            finished = true;

            QString message;
            switch (result) {
            case OV_EREAD: message = "VORBIS DECODER INIT ERROR:  A read from media returned an error.";
                break;
            case OV_ENOTVORBIS: message = "VORBIS DECODER INIT ERROR:  Bitstream does not contain any Vorbis data.";
                break;
            case OV_EVERSION: message = "VORBIS DECODER INIT ERROR: Vorbis version mismatch";
                break;
            case OV_EBADHEADER: message = "VORBIS DECODER INIT ERROR: Invalid Vorbis bitstream header.";
                break;
            case OV_EFAULT: message = "VORBIS DECODER INIT ERROR: Internal logic fault; indicates a bug or heap/stack corruption.";
            }
            qWarning() << message;
        }
	}
    return initialized;
}

bool VorbisDecoder::hasMoreSamplesToDecode(){
    return isInitialized() && !finished;
}
