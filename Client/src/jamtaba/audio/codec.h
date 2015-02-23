#pragma once

#include <memory>
#include <QByteArray>

extern "C"{
    #include "minimp3/include/minimp3.h"
}

namespace Audio{

class SamplesBuffer;


class Mp3Decoder
{
public:
    virtual const Audio::SamplesBuffer* decode(char* inputBuffer, int bytesToDecode) = 0;
    virtual void reset() = 0;
    virtual ~Mp3Decoder(){}
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Mp3DecoderMiniMp3 : public Mp3Decoder{

public:
    Mp3DecoderMiniMp3();
    ~Mp3DecoderMiniMp3();
    virtual const Audio::SamplesBuffer* decode(char *inputBuffer, int bytesToDecode);
    virtual void reset();
private:
    static const int MINIMUM_SIZE_TO_DECODE;// = 1024 + 256;
    static const int AUDIO_SAMPLES_BUFFER_MAX_SIZE;// = 4096 * 4;
    static const int INTERNAL_SHORT_BUFFER_SIZE;
    mp3_decoder_t mp3Decoder;
    mp3_info_t mp3Info;
    signed short* internalShortBuffer;
    Audio::SamplesBuffer* buffer;
    Audio::SamplesBuffer* NULL_BUFFER;
    QByteArray array;
};

}
