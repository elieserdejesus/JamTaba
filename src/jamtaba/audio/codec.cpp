#include "codec.h"
#include <climits>
#include "core/AudioDriver.h"
#include <QDebug>
#include <cmath>

using namespace Audio;

Mp3DecoderMiniMp3::Mp3DecoderMiniMp3()
    :mp3Decoder(mp3_create()), buffer(nullptr)

{
    int size = MP3_MAX_SAMPLES_PER_FRAME*8 * 2;
    internalShortBuffer = new signed short[size];//recommend by the minimp3 author
    for (int i = 0; i < size; ++i) {
        internalShortBuffer[i] = 0;
    }
    NULL_BUFFER = new Audio::AudioSamplesBuffer(1, 0);
}

const AudioSamplesBuffer* Mp3DecoderMiniMp3::decode(char *inputBuffer, int bytesToDecode){
    array.append(inputBuffer, bytesToDecode);
    if(array.size() < MINIMUM_SIZE_TO_DECODE ){
        return NULL_BUFFER;
    }
    //qDebug() << "array.size:"<<array.size() << "bytesToDecode:"<<bytesToDecode;
    int totalBytesDecoded = 0;
    int bytesDecoded = 0;
    signed short* out = internalShortBuffer;
    char* in = array.data();
    int totalSamplesDecoded = 0;
    do{
        int bytesLeft = bytesToDecode - totalBytesDecoded;
        bytesDecoded = mp3_decode((void**)mp3Decoder, in, bytesLeft, out, &mp3Info );
        if(bytesDecoded > 0){
            //qDebug() << "decoding stream sampleRate:" << mp3Info.sample_rate << " channels:" << mp3Info.channels;
            bytesLeft -= bytesDecoded;
            in += bytesDecoded;
            int samplesDecoded = mp3Info.audio_bytes/2;
            out += samplesDecoded;
            totalSamplesDecoded += samplesDecoded;
            totalBytesDecoded += bytesDecoded;
        }
    }
    while(bytesDecoded > 0 && totalBytesDecoded < bytesToDecode);
    array = array.right(array.size() - totalBytesDecoded);//keep just the undecoded bytes to the next call for decode
    if(totalBytesDecoded <= 0){
        return NULL_BUFFER;
    }
    //+++++++++++++++++++++++++++
    int framesDecoded = totalSamplesDecoded/mp3Info.channels;

    if(!buffer){
        buffer = new Audio::AudioSamplesBuffer(mp3Info.channels, AUDIO_SAMPLES_BUFFER_MAX_SIZE);
    }
    //Q_ASSERT(framesDecoded <= AUDIO_SAMPLES_BUFFER_MAX_SIZE);
    if(framesDecoded > AUDIO_SAMPLES_BUFFER_MAX_SIZE){
        framesDecoded = AUDIO_SAMPLES_BUFFER_MAX_SIZE;
    }
    buffer->setFrameLenght( framesDecoded);
    int internalIndex = 0;
    for (int i = 0; i < framesDecoded; ++i) {
        for (int c = 0; c < buffer->getChannels(); ++c) {
            buffer->set(c, i, (float)internalShortBuffer[internalIndex++] / SHRT_MAX);
        }
    }

    return buffer;
}

Mp3DecoderMiniMp3::~Mp3DecoderMiniMp3(){
    delete [] internalShortBuffer;
    delete buffer;
}

