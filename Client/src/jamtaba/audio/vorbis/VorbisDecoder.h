//#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include "../audio/core/SamplesBuffer.h"
#include <QByteArray>

#ifndef VORBIS_DECODER_H
#define VORBIS_DECODER_H


class VorbisDecoder{
public:
    VorbisDecoder();
    ~VorbisDecoder();
    const Audio::SamplesBuffer& decode(int maxSamplesToDecode);

    inline bool isStereo() const{return getChannels() == 2;}
    inline bool isMono() const{return vorbisFile.vi->channels == 1;}
    inline int getChannels() const{return vorbisFile.vi->channels;}
    inline int getSampleRate() const{return vorbisFile.vi->rate;}

    void setInput(QByteArray vorbisData);
    void reset();
private:
    bool initialize();
    Audio::SamplesBuffer internalBuffer;
    OggVorbis_File vorbisFile;
    bool initialized;
    QByteArray vorbisInput;

    static size_t readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *decoderInstance);

    int consumeTo(void* oggOutBuffer, int bytesToConsume);

};



#endif
