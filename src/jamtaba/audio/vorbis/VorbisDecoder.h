#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include <QByteArray>

#ifndef VORBIS_DECODER_H
#define VORBIS_DECODER_H

class VorbisDecoder{
public:
    VorbisDecoder(QByteArray &input);
	~VorbisDecoder();
    int decode(float ***outBuffer, int maxSamplesToDecode);
	bool hasMoreSamplesToDecode();
	void reset();
    inline bool isStereo() const{return getChannels() == 2;}
    inline bool isMono() const{return info->channels == 1;}
    inline int getChannels() const{return info->channels;}
    inline int getSampleRate() const{return info->rate;}
    void initialize();

    inline bool isInitialized() const{return info;}
private:
    OggVorbis_File vorbisFile;
    vorbis_info* info;// struct that stores all the static vorbis bitstream settings

	bool finished;// = false;

    QByteArray& input;

    static size_t readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *datasource);

};



#endif
