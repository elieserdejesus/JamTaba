#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <QByteArray>

#ifndef VORBIS_DECODER_H
#define VORBIS_DECODER_H

namespace Audio {
    class SamplesBuffer;
}

class VorbisDecoder{
public:
    VorbisDecoder();
    ~VorbisDecoder();
    const Audio::SamplesBuffer* decode(int maxSamplesToDecode);

    bool hasMoreSamplesToDecode();

    inline bool isStereo() const{return getChannels() == 2;}
    inline bool isMono() const{return vorbisFile.vi->channels == 1;}
    inline int getChannels() const{return vorbisFile.vi->channels;}
    inline int getSampleRate() const{return vorbisFile.vi->rate;}


    inline bool isInitialized() const{return initialized;}

    void reset();

    void addVorbisData(QByteArray vorbisData, bool isLastPart);

    inline bool canDecode() const{return lastPartAdded;}

private:
    bool initialize();
    Audio::SamplesBuffer* internalBuffer;
    OggVorbis_File vorbisFile;
    bool initialized;
	bool finished;// = false;
    bool lastPartAdded;

    QByteArray vorbisInput;

    static size_t readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *datasource);

};



#endif
