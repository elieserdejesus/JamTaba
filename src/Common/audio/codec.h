#ifndef JT_CODEC_H
#define JT_CODEC_H

extern "C" { // this give me a error in linux
    #include "minimp3.h"
}
#include <QByteArray>

namespace Audio {
class SamplesBuffer;

class Mp3Decoder
{
public:
    virtual const Audio::SamplesBuffer *decode(char *inputBuffer, int bytesToDecode) = 0;
    virtual void reset() = 0;
    virtual int getSampleRate() const = 0;
    virtual ~Mp3Decoder()
    {
    }
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Mp3DecoderMiniMp3 : public Mp3Decoder
{
public:
    Mp3DecoderMiniMp3();
    ~Mp3DecoderMiniMp3();
    virtual const Audio::SamplesBuffer *decode(char *inputBuffer, int inputBufferLenght);
    virtual void reset();
    virtual int getSampleRate() const;
private:
    static const int MINIMUM_SIZE_TO_DECODE;
    static const int AUDIO_SAMPLES_BUFFER_MAX_SIZE;
    static const int INTERNAL_SHORT_BUFFER_SIZE;
    mp3_decoder_t mp3Decoder;
    mp3_info_t
        mp3Info;
    signed short *internalShortBuffer;
    Audio::SamplesBuffer *buffer;
    Audio::SamplesBuffer *NULL_BUFFER;
    QByteArray array;
};
}

#endif
