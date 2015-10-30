#include "VorbisDecoder.h"
#include <stdexcept>
#include <cstring>
#include <QByteArray>
#include <cmath>
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include "../audio/core/SamplesBuffer.h"
#include <vorbis/vorbisfile.h>
#include <QThread>
#include "../log/logging.h"
//+++++++++++++++++++++++++++++++++++++++++++
VorbisDecoder::VorbisDecoder()
    : internalBuffer(2, 4096),
      initialized(false),
      vorbisInput()
{
    outBuffer = new float*[2];
    outBuffer[0] = new float[2048];
    outBuffer[1] = new float[2048];

    decodedSamples = 0;
}
//+++++++++++++++++++++++++++++++++++++++++++
VorbisDecoder::~VorbisDecoder(){
    qCDebug(jtNinjamVorbisDecoder) << "Destrutor Vorbis Decoder";
    //TODO this destructor is crashing when a track is removed
    //delete [] outBuffer[0];
    //delete [] outBuffer[1];
    //delete [] outBuffer;
    //if(initialized){
//        ov_clear(&vorbisFile);
//    }
}
//+++++++++++++++++++++++++++++++++++++++++++
size_t VorbisDecoder::consumeTo(void *oggOutBuffer, size_t bytesToConsume){
    size_t len = qMin( bytesToConsume, (size_t)vorbisInput.size());
    if(len > 0){
        memcpy(oggOutBuffer, vorbisInput.data(), len);
        vorbisInput.remove(0, (uint)len);
    }
//    else{
//        qCritical() << "len " << len;
//    }
    return len;
}

//vorbisfile read callback
size_t VorbisDecoder::readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *decoder){
    VorbisDecoder* decoderInstance = reinterpret_cast<VorbisDecoder*>(decoder);
    return decoderInstance->consumeTo(oggOutBuffer, size * nmemb);
}
//+++++++++++++++++++++++++++++++++++++++++++
const Audio::SamplesBuffer &VorbisDecoder::decode(int maxSamplesToDecode){
    if(!initialized){
        initialize();
    }
    if(!initialized){
        return Audio::SamplesBuffer::ZERO_BUFFER;
    }
    //static float decoderOutBuffer[vorbisFile.vi->channels]
    long samplesDecoded = ov_read_float(&vorbisFile, &outBuffer, maxSamplesToDecode, NULL);//currentSection is not used
    if(samplesDecoded < 0){//error
        QString message;
        switch (samplesDecoded) {
            case OV_HOLE: message = "VORBIS ERROR: there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)";
                break;
            case OV_EBADLINK: message = "VORBIS ERROR: an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.";
                break;
            case OV_EINVAL: message = "VORBIS ERROR: the initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.";
        }
        qCWarning(jtNinjamVorbisDecoder) << message;
        return Audio::SamplesBuffer::ZERO_BUFFER;
    }
    internalBuffer.setFrameLenght(samplesDecoded);
    decodedSamples += samplesDecoded;
    //internal buffer is always stereo
    internalBuffer.add(0, outBuffer[0], samplesDecoded);//the left channel is always copyed
    internalBuffer.add(1, outBuffer[ (vorbisFile.vi->channels >= 2) ? 1 : 0 ], samplesDecoded);
    return internalBuffer;
}
//+++++++++++++++++++++++++++++++++++++++++++
void VorbisDecoder::reset(){
    initialize();
    decodedSamples = 0;
    //qWarning() << "reset";
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

    if(initialized){
        ov_clear(&vorbisFile);
    }

    int result = ov_open_callbacks((void*)this, &vorbisFile, NULL, 0, callbacks );
    
	initialized = result == 0;

    if(!initialized){
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
        qCWarning(jtNinjamVorbisDecoder) << message;
    }
    return initialized;
}
