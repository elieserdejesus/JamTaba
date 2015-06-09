#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <QByteArray>

#ifndef VORBIS_DECODER_H
#define VORBIS_DECODER_H

class VorbisDecoder{
public:
    VorbisDecoder(QByteArray &input);
	~VorbisDecoder();
    int decode(char* outBuffer);
	bool hasMoreSamplesToDecode();
	void reset();
    inline bool isStereo() const{return getChannels() == 2;}
    inline bool isMono() const{return info.channels == 1;}
    inline int getChannels() const{return info.channels;}
    inline int getSampleRate() const{return info.rate;}
    void initialize();

    inline bool isInitialized() const{return headerParsed;}
private:

	ogg_sync_state syncState;// sync and verify incoming physical bitstream
	ogg_stream_state streamState;// take physical pages, weld into a logical stream of packets
	ogg_page page; // one Ogg bitstream page.  Vorbis packets are inside
	ogg_packet packet;// one raw packet of data for decode

	vorbis_info info;// struct that stores all the static vorbis bitstream settings
	vorbis_comment comment; // struct that stores all the bitstream user comments
	vorbis_dsp_state dspState; // central working state for the packet->PCM decoder
	vorbis_block block; // local working space for packet->PCM decode

	bool decodingPackets;// = false;
	int lastPageOut;
	int lastPacketOut;

	bool headerParsed;// = false;

    bool parseHeaders();

    int readFromInputStream();

    int decodePacket(char *outBuffer);

	bool finished;// = false;
	int convsize;// = 4096;

    QByteArray& input;
    //jobject decodeResult;//used to return de decoded samples, channels and sample rate

};



#endif
