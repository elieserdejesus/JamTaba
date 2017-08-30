#include <vorbis/vorbisfile.h>
#include "audio/core/SamplesBuffer.h"
#include <QByteArray>

#ifndef VORBIS_DECODER_H
#define VORBIS_DECODER_H

class VorbisDecoder
{

public:

    VorbisDecoder();
    ~VorbisDecoder();
    const Audio::SamplesBuffer &decode(int maxSamplesToDecode);

    bool isStereo() const;

    bool isMono() const;

    int getChannels() const;

    int getSampleRate() const;

    bool isInitialized() const;

    void setInputData(const QByteArray &vorbisData);

    bool initialize();

private:

    Audio::SamplesBuffer internalBuffer;
    OggVorbis_File vorbisFile;
    bool initialized;
    QByteArray vorbisInput;
    float **outBuffer;
    static size_t readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *decoderInstance);

    size_t consumeTo(void *oggOutBuffer, size_t bytesToConsume);
};

inline bool VorbisDecoder::isStereo() const
{
    return getChannels() == 2;
}

inline bool VorbisDecoder::isMono() const
{
    return vorbisFile.vi->channels == 1;
}

inline int VorbisDecoder::getChannels() const
{
    return vorbisFile.vi->channels;
}

inline int VorbisDecoder::getSampleRate() const
{
    if (vorbisFile.vi)
        return vorbisFile.vi->rate;
    return 44100;
}

inline bool VorbisDecoder::isInitialized() const
{
    return initialized;
}

#endif
