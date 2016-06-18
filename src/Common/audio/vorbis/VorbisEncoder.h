#ifndef VORBISENCODER_H
#define VORBISENCODER_H

#include "vorbis/vorbisenc.h"
#include "audio/core/SamplesBuffer.h"
#include <QByteArray>

class VorbisEncoder
{

public:
    VorbisEncoder();
    VorbisEncoder(int channels, int sampleRate, float quality);
    ~VorbisEncoder();

    QByteArray encode(const Audio::SamplesBuffer& in);
    QByteArray finishIntervalEncoding();
    inline int getChannels() const{return info.channels;}
    inline int getSampleRate() const{return info.rate;}

    static const float QUALITY_LOW;
    static const float QUALITY_NORMAL;
    static const float QUALITY_HIGH;

private:
    float quality;
    ogg_stream_state streamState; /* take physical pages, weld into a logical stream of packets */
    vorbis_info      info; /* struct that stores all the static vorbis bitstream settings */
    vorbis_comment   comment; /* struct that stores all the user comments */

    vorbis_dsp_state dspState; /* central working state for the packet->PCM decoder */
    vorbis_block     block; /* local working space for packet->PCM decode */

    int totalEncoded;

    bool initialized;

    QByteArray outBuffer;

    void init(int channels, int sampleRate, float quality);

    void encodeFirstVorbisHeaders();
    void clearState();

    bool isFirstEncoding;

    int streamID;
};



#endif // VORBISENCODER_H
