#ifndef VORBISENCODER_H
#define VORBISENCODER_H

#include "vorbis/vorbisenc.h"
#include "../audio/core/SamplesBuffer.h"
#include <QByteArray>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(vorbisEncoder)

class VorbisEncoder
{

public:
    VorbisEncoder();
    VorbisEncoder(int channels, int sampleRate);
    ~VorbisEncoder();

    QByteArray encode(const Audio::SamplesBuffer& in);
    QByteArray finishIntervalEncoding();
    inline int getChannels() const{return info.channels;}
        inline int getTotalEncoded() const{return totalEncoded;}
private:

    int totalEncoded;

    ogg_stream_state streamState; /* take physical pages, weld into a logical stream of packets */
    ogg_page         page; /* one Ogg bitstream page.  Vorbis packets are inside */
    ogg_packet       packet; /* one raw packet of data for decode */

    vorbis_info      info; /* struct that stores all the static vorbis bitstream settings */
    vorbis_comment   comment; /* struct that stores all the user comments */

    vorbis_dsp_state dspState; /* central working state for the packet->PCM decoder */
    vorbis_block     block; /* local working space for packet->PCM decode */

    void initializeVorbis();

    //bool finishIntervalRequested;
    bool initialized;

    QByteArray outBuffer;

    void init(int channels, int sampleRate);

};



#endif // VORBISENCODER_H
