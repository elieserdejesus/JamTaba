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
      vorbisInput(),
      initialized(false)
{

}
//+++++++++++++++++++++++++++++++++++++++++++
VorbisDecoder::~VorbisDecoder(){
    if(initialized){
        ov_clear(&vorbisFile);
    }
    if(internalBuffer){
        delete internalBuffer;
    }
}
//+++++++++++++++++++++++++++++++++++++++++++
int VorbisDecoder::consumeTo(void *oggOutBuffer, int bytesToConsume){
    int len = std::min( bytesToConsume, vorbisInput.size());
    if(len > 0){
        memcpy(oggOutBuffer, vorbisInput.data(), len);
        vorbisInput.remove(0, len);
    }
//    else{
//        finished = true;
//    }
    return len;
}

//vorbisfile read callback
size_t VorbisDecoder::readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *decoder){
    VorbisDecoder* decoderInstance = reinterpret_cast<VorbisDecoder*>(decoder);
    return decoderInstance->consumeTo(oggOutBuffer, size * nmemb);
}
//+++++++++++++++++++++++++++++++++++++++++++
const Audio::SamplesBuffer* VorbisDecoder::decode(int maxSamplesToDecode){
//    if(!canDecode()){//wait to complete the download of encoded interval
//        return &(Audio::SamplesBuffer::ZERO_BUFFER);
//    }
    if(!initialized){
        initialize();
    }
    long samplesDecoded = ov_read_float(&vorbisFile, &internalBuffer->samples, maxSamplesToDecode, NULL);//currentSection is not used
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
    //if(samplesDecoded == 0){//sometimes decoder can't decode samples because don't have enough bytes to decode
        //qWarning() << "samples decoded == zero!";
    //}

//    if(totalBytesInLastInterval > 0 && bytesConsumed >= totalBytesInLastInterval){
//        reset();//prepare to decode a new interval
//    }
    return internalBuffer;
}
//+++++++++++++++++++++++++++++++++++++++++++
void VorbisDecoder::reset(){
    if(initialized){
        ov_clear(&vorbisFile);
    }
    initialize();
}
//++++++++++++++++++++++++++++++++++++++++++++++
void VorbisDecoder::setInput(QByteArray vorbisData){
    vorbisInput.clear();
    vorbisInput.append(vorbisData);
}

//+++++++++++++++++++++++++++++++++++++++++++
bool VorbisDecoder::initialize(){
    ov_callbacks callbacks;
    callbacks.read_func = readOgg;
    callbacks.seek_func = NULL;
    callbacks.close_func = NULL;
    callbacks.tell_func = NULL;
    int result = ov_open_callbacks((void*)this, &vorbisFile, NULL, 0, callbacks );
    if(result == 0){
        if(internalBuffer){
            if(internalBuffer->getChannels() != vorbisFile.vi->channels){
                delete internalBuffer;
                internalBuffer = new Audio::SamplesBuffer(vorbisFile.vi->channels, 4096);
            }
        }
        else{
            internalBuffer = new Audio::SamplesBuffer(vorbisFile.vi->channels, 4096);
        }
        initialized = true;
    }
    else{
        initialized = false;
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
        return false;
    }

    return initialized;
}

//bool VorbisDecoder::canDecode() const{
//    return vorbisInput.size() >= 4096 * 2;
//}


