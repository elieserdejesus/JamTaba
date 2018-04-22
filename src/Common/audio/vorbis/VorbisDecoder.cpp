#include "VorbisDecoder.h"
#include <stdexcept>
#include <cstring>
#include <QByteArray>
#include <cmath>
#include <QDebug>
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include <vorbis/vorbisfile.h>
#include <QThread>
#include "log/Logging.h"

using vorbis::Decoder;

Decoder::Decoder() :
      internalBuffer(2, 4096),
      initialized(false),
      vorbisInput()
{
    vorbisFile.vi = nullptr;
}

Decoder::~Decoder()
{
    qCDebug(jtNinjamVorbisDecoder) << "Destrutor Vorbis Decoder";

    if(initialized)
        ov_clear(&vorbisFile);
}

bool Decoder::isMono() const
{
    if (vorbisFile.vi)
        return vorbisFile.vi->channels == 1;

    return true;
}

int Decoder::getChannels() const
{
    if (vorbisFile.vi)
        return vorbisFile.vi->channels;

    return 1;
}

int Decoder::getSampleRate() const
{
    if (vorbisFile.vi)
        return vorbisFile.vi->rate;

    return 44100;
}


//+++++++++++++++++++++++++++++++++++++++++++
size_t Decoder::consumeTo(void *oggOutBuffer, size_t bytesToConsume){
    size_t len = qMin( bytesToConsume, (size_t)vorbisInput.size());
    if (len > 0) {
        memcpy(oggOutBuffer, vorbisInput.data(), len);
        vorbisInput.remove(0, (uint)len);
    }

    return len;
}

//vorbisfile read callback
size_t Decoder::readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *decoder)
{
    vorbis::Decoder* decoderInstance = reinterpret_cast<vorbis::Decoder*>(decoder);
    return decoderInstance->consumeTo(oggOutBuffer, size * nmemb);
}

const audio::SamplesBuffer &Decoder::decode(int maxSamplesToDecode)
{
    if (!initialized) {
        initialize();
    }

    if (!initialized) {
        return audio::SamplesBuffer::ZERO_BUFFER;
    }

    float **outBuffer;

    long samplesDecoded = ov_read_float(&vorbisFile, &outBuffer, maxSamplesToDecode, NULL);//currentSection is not used
    if (samplesDecoded < 0) { //error
        QString message;
        switch (samplesDecoded) {
            case OV_HOLE: message = "VORBIS ERROR: there was an interruption in the data. (one of: garbage between pages, loss of sync followed by recapture, or a corrupt page)";
                break;
            case OV_EBADLINK: message = "VORBIS ERROR: an invalid stream section was supplied to libvorbisfile, or the requested link is corrupt.";
                break;
            case OV_EINVAL: message = "VORBIS ERROR: the initial file headers couldn't be read or are corrupt, or that the initial open call for vf failed.";
        }
        qCWarning(jtNinjamVorbisDecoder) << message;
        return audio::SamplesBuffer::ZERO_BUFFER;
    }
    internalBuffer.setFrameLenght(samplesDecoded);
    //internal buffer is always stereo
    if (samplesDecoded > 0) {
        internalBuffer.add(0, outBuffer[0], samplesDecoded);//the left channel is always copyed
        internalBuffer.add(1, outBuffer[ (vorbisFile.vi->channels >= 2) ? 1 : 0 ], samplesDecoded);
    }
    else {
        internalBuffer.zero();
    }

    return internalBuffer;
}

void Decoder::setInputData(const QByteArray &vorbisData)
{
    vorbisInput.clear();
    vorbisInput.append(vorbisData);
}

bool Decoder::initialize()
{
    ov_callbacks callbacks;
    callbacks.read_func = readOgg;
    callbacks.seek_func = NULL;
    callbacks.close_func = NULL;
    callbacks.tell_func = NULL;

    if (initialized) {
        ov_clear(&vorbisFile);
    }

    int result = ov_open_callbacks((void*)this, &vorbisFile, NULL, 0, callbacks );
    
	initialized = result == 0;

    if (!initialized) {
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
