#ifndef VORBISENCODER_H
#define VORBISENCODER_H

#include "vorbis/vorbisenc.h"

#include "audio/core/SamplesBuffer.h"
#include "audio/Encoder.h"

#include <QByteArray>

namespace vorbis
{

class Encoder : public AudioEncoder
{

public:
    Encoder();
    Encoder(uint channels, uint sampleRate, float quality);
    ~Encoder();

    QByteArray encode(const Audio::SamplesBuffer &audioBuffer) override;
    QByteArray finishIntervalEncoding() override;

    int getChannels() const;
    int getSampleRate() const;

private:

    ogg_stream_state streamState;   // take physical pages, weld into a logical stream of packets
    vorbis_info      info;          // struct that stores all the static vorbis bitstream settings
    vorbis_comment   comment;       // struct that stores all the user comments

    vorbis_dsp_state dspState;      // central working state for the packet->PCM decoder
    vorbis_block     block;         // local working space for packet->PCM decode

    int totalEncoded;

    bool initialized;

    QByteArray outBuffer;

    void init(uint channels, uint sampleRate, float quality);

    void encodeFirstVorbisHeaders();
    void clearState();

    bool isFirstEncoding;

    int streamID;
};


inline int Encoder::getChannels() const
{
    return info.channels;
}

inline int Encoder::getSampleRate() const
{
    return info.rate;
}

} // namespace

#endif // VORBISENCODER_H
