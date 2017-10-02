#ifndef JT_CODEC_H
#define JT_CODEC_H

extern "C" { // this give me a error in linux
    #include "minimp3.h"
}

#include <QByteArray>
#include "audio/core/SamplesBuffer.h"

namespace Audio {

class Mp3Decoder
{
public:
    virtual const Audio::SamplesBuffer decode(char *inputBuffer, int bytesToDecode) = 0;
    virtual void reset() = 0;
    virtual int getSampleRate() const = 0;
    virtual ~Mp3Decoder();
};

inline Mp3Decoder::~Mp3Decoder()
{
    //
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class Mp3DecoderMiniMp3 : public Mp3Decoder
{

public:
    Mp3DecoderMiniMp3();
    ~Mp3DecoderMiniMp3();
    const Audio::SamplesBuffer decode(char *inputBuffer, int inputBufferLenght) override;
    void reset() override;
    int getSampleRate() const override;

private:
    static const int MINIMUM_SIZE_TO_DECODE;
    static const int AUDIO_SAMPLES_BUFFER_MAX_SIZE;
    static const int INTERNAL_SHORT_BUFFER_SIZE = MP3_MAX_SAMPLES_PER_FRAME * 8 * 2; // recommended by minimp3 author

    mp3_decoder_t mp3Decoder;
    mp3_info_t mp3Info;
    signed short internalShortBuffer[INTERNAL_SHORT_BUFFER_SIZE];
    Audio::SamplesBuffer buffer;
    QByteArray array;
};

} // namespace

#endif
