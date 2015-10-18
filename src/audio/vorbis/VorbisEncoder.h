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
    inline int getSampleRate() const{return info.rate;}
//    inline int getTotalEncoded() const{return totalEncoded;}
private:
    static const float QUALITY;// = 0.32;//vorbis default quality is 0.3
    ogg_stream_state streamState; /* take physical pages, weld into a logical stream of packets */
    vorbis_info      info; /* struct that stores all the static vorbis bitstream settings */
    vorbis_comment   comment; /* struct that stores all the user comments */

    vorbis_dsp_state dspState; /* central working state for the packet->PCM decoder */
    vorbis_block     block; /* local working space for packet->PCM decode */

    int totalEncoded;

    //void initializeVorbis();

    //bool finishIntervalRequested;
    bool initialized;

    QByteArray outBuffer;

    void init(int channels, int sampleRate);

    //void writeVorbisHeaderInOutputBuffer();
    //void clearStreamResources();
    //void initializeAnalysisResources();
    //int streamSerialNumber;

    void encodeFirstVorbisHeaders();
    void clearState();

    bool isFirstEncoding;

    int streamID;
};



#endif // VORBISENCODER_H
