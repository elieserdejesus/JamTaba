#include "VorbisDecoder.h"
#include <stdexcept>
#include <cstring>
#include <QByteArray>
#include <cmath>
#include <QDebug>
#include <vorbis/vorbisfile.h>

VorbisDecoder::VorbisDecoder(QByteArray& input)
    : input(input), info(nullptr)
{
    this->finished = false;

}

VorbisDecoder::~VorbisDecoder(){
    if(info){
        ov_clear(&vorbisFile);
    }
}

size_t VorbisDecoder::readOgg(void *oggOutBuffer, size_t size, size_t nmemb, void *datasource)
{
    QByteArray* byteArray = reinterpret_cast<QByteArray*>(datasource);
    if(byteArray->isEmpty()){
        return 0;//end of stream
    }
    size_t len = std::min( (int)(size * nmemb), byteArray->size());
    memcpy(oggOutBuffer, byteArray->data(), len);
    byteArray->remove(0, len);
    return len;
}

void VorbisDecoder::reset(){
    finished = false;

}

//return decoded bytes
int VorbisDecoder::decode(float*** outBuffer, int maxSamplesToDecode){
    if (!isInitialized()){
		initialize();
	}
    static int currentSection;
    long samplesDecoded = ov_read_float(&vorbisFile, outBuffer, maxSamplesToDecode, &currentSection);//currentSection is not used
    if(samplesDecoded == 0){
        finished = true;
    }
    return samplesDecoded;
}

void VorbisDecoder::initialize(){
    if (!isInitialized()){
        ov_callbacks callbacks;
        callbacks.read_func = readOgg;
        callbacks.seek_func = NULL;
        callbacks.close_func = NULL;
        callbacks.tell_func = NULL;

        int result = ov_open_callbacks((void*)&input, &vorbisFile, NULL, 0, callbacks );
        if(result < 0){
            qCritical() << "Erro no openCallBacks" << result;
        }
        info = ov_info(&vorbisFile, -1);
	}
}

bool VorbisDecoder::hasMoreSamplesToDecode(){
	return !finished;
}
