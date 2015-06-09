#include "VorbisDecoder.h"
#include <stdexcept>
#include <cstring>
#include <QByteArray>
#include <cmath>
#include <QDebug>

VorbisDecoder::VorbisDecoder(QByteArray& input)
    : input(input)
{
    this->decodingPackets = this->headerParsed = this->finished = false;
    this->convsize = 4096;
	ogg_sync_init(&syncState);
    //initialize();//parse the ogg headers
}

VorbisDecoder::~VorbisDecoder(){

}

void VorbisDecoder::reset(){
	finished = headerParsed = false;
	vorbis_block_clear(&block);
	vorbis_dsp_clear(&dspState);

	ogg_stream_clear(&streamState);
	vorbis_comment_clear(&comment);
	vorbis_info_clear(&info);  /* must be called last */

	ogg_sync_clear(&syncState);
	ogg_sync_init(&syncState);
}

//return decoded bytes
int VorbisDecoder::decode(char *outBuffer){
	if (!headerParsed){
		initialize();
	}
    //daqui para baixo apenas copiei o código java
	if (finished) {
        //env->CallObjectMethod(outBuffer, byteBufferLimitIMethodD, 0);
        return 0;
	}

	int bytesReaded = 0;
	if (!decodingPackets) {
		do {
			lastPageOut = ogg_sync_pageout(&syncState, &page);
			if (lastPageOut != 1) {
                bytesReaded = readFromInputStream();
			}
		} while (lastPageOut != 1 && bytesReaded != -1);
	}

	if (lastPageOut == 0 && bytesReaded == -1){
		finished = true;
        //env->CallObjectMethod(outBuffer, byteBufferLimitIMethodD, 0);
        return 0;
	}

	if (lastPageOut == 1) {
		if (!decodingPackets) {
			ogg_stream_pagein(&streamState, &page);
			decodingPackets = true;
		}

		if (ogg_stream_packetout(&streamState, &packet) == 1) {//If we have the data we need, we decode the packet.
            int decodedBytes = decodePacket(outBuffer);
            //env->CallObjectMethod(outBuffer, byteBufferLimitIMethodD, decodedBytes);
            return decodedBytes;
		}
		else {
			decodingPackets = false;
		}
	}
    //env->CallObjectMethod(outBuffer, byteBufferLimitIMethodD, 0);
    return 0;

}

int VorbisDecoder::decodePacket(char* outBuffer) {
	if (vorbis_synthesis(&block, &packet) == 0){ /* test for success! */
		vorbis_synthesis_blockin(&dspState, &block);
	}

	float **pcm;
	int samples = 0;

	if ((samples = vorbis_synthesis_pcmout(&dspState, &pcm)) > 0){
		//float[][] pcm = _pcm[0];//pcm[0] is left, and pcm[1] is right.
		int bout = (samples < convsize ? samples : convsize);
		int bytesToWritePerChannel = bout * sizeof(float);
        memcpy(outBuffer, pcm[0], bytesToWritePerChannel);
		if (info.channels > 1){
            memcpy(outBuffer + bytesToWritePerChannel, pcm[1], bytesToWritePerChannel);
		}
		vorbis_synthesis_read(&dspState, bout);// tell libvorbis how many samples we actually consumed 
        return bytesToWritePerChannel * info.channels;//return writed bytes
	}
	return 0;
}

int VorbisDecoder::readFromInputStream(){
    int bytes = std::min(4096, input.size());
    char* oggBuffer = ogg_sync_buffer(&syncState, bytes);
    memcpy(oggBuffer, input.data(), bytes);
	if (bytes > 0){
        ogg_sync_wrote(&syncState, bytes);
	}
    return bytes;
}

void VorbisDecoder::initialize(){
    if (!isInitialized()){
        headerParsed = parseHeaders();
		if (headerParsed){
			//init vorbis
			if (vorbis_synthesis_init(&dspState, &info) != 0){
                qWarning() << "Error in vorbis_synthesis_init";
			}
			vorbis_block_init(&dspState, &block);
			convsize = 4096 / info.channels;

			//invoke listener
            //env->CallVoidMethod(decoderInputReader, decoderInputReaderHeaderInfosAvailableMethod, (jlong)info.rate, (jint)info.channels);
            //if (env->ExceptionOccurred()){
                //return;
            //}
		}
		else{
            qWarning() << "Error parsing header!";
		}
	}
}

bool VorbisDecoder::parseHeaders(){
	//int eos = 0;
	int i;

	/* submit a block to libvorbis' Ogg layer */
    if (readFromInputStream() < 0){
        throw std::domain_error( "Error when try read the first bytes in stream");
	}

	/* Get the first page. */
	if (ogg_sync_pageout(&syncState, &page) != 1){
		/* have we simply run out of data?  If so, we're done. */
		//if (bytes<4096)break;
        qWarning() << "Input does not appear to be an Ogg bitstream.\n";
		return false;
	}

	ogg_stream_init(&streamState, ogg_page_serialno(&page));

	vorbis_info_init(&info);
	vorbis_comment_init(&comment);
	if (ogg_stream_pagein(&streamState, &page) < 0){
        qWarning() << "Error reading first page of Ogg bitstream data.\n";
		return false;
	}

	if (ogg_stream_packetout(&streamState, &packet) != 1){
		/* no page? must not be vorbis */
        qWarning() << "Error reading initial header packet.\n";
		return false;
	}

	if (vorbis_synthesis_headerin(&info, &comment, &packet) < 0){
		/* error case; not a vorbis header */
        qWarning() << "This Ogg bitstream does not contain Vorbis audio data.\n";
		return false;
	}

	/* At this point, we're sure we're Vorbis. We've set up the logical
	(Ogg) bitstream decoder. Get the comment and codebook headers and
	set up the Vorbis decoder */

	/* The next two packets in order are the comment and codebook headers.
	They're likely large and may span multiple pages. Thus we read
	and submit data until we get our two packets, watching that no
	pages are missing. If a page is missing, error out; losing a
	header page is the only place where missing data is fatal. */

	i = 0;
	while (i < 2){
		while (i < 2){
			int result = ogg_sync_pageout(&syncState, &page);
			if (result == 0)break; /* Need more data */
			/* Don't complain about missing or corrupt data yet. We'll catch it at the packet output phase */
			if (result == 1){
				ogg_stream_pagein(&streamState, &page);
				while (i < 2){
					result = ogg_stream_packetout(&streamState, &packet);
					if (result == 0)break;
					if (result < 0){
                        qWarning() << "Corrupt secondary header.  Exiting.\n";
						return false;
					}
					result = vorbis_synthesis_headerin(&info, &comment, &packet);
					if (result < 0){
                        qWarning()<<"Corrupt secondary header.  Exiting.\n";
						return false;
					}
					i++;
				}
			}
		}
        readFromInputStream();

	}
	return true;

}

bool VorbisDecoder::hasMoreSamplesToDecode(){
	return !finished;
}
