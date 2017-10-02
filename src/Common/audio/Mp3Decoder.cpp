#include "Mp3Decoder.h"
#include "core/AudioDriver.h"
#include "core/SamplesBuffer.h"

#include <QDebug>
#include <cmath>
#include <climits>

using namespace Audio;

const int Mp3DecoderMiniMp3::MINIMUM_SIZE_TO_DECODE = 1024 + 256;
const int Mp3DecoderMiniMp3::AUDIO_SAMPLES_BUFFER_MAX_SIZE = 4096 * 2;

Mp3DecoderMiniMp3::Mp3DecoderMiniMp3() :
    mp3Decoder(nullptr),
    buffer(SamplesBuffer(2, INTERNAL_SHORT_BUFFER_SIZE))
{
    mp3Decoder = mp3_create();
    reset();
}

Mp3DecoderMiniMp3::~Mp3DecoderMiniMp3()
{
    // leaking here to avoid a crash, minimp3 has a litle problem in mp3_done described in author blog comments: http://keyj.emphy.de/minimp3/

    //if (mp3Decoder)
        //mp3_done(&mp3Decoder);
}

void Mp3DecoderMiniMp3::reset()
{
    array.clear();

    for (int i = 0; i < INTERNAL_SHORT_BUFFER_SIZE; ++i)
        internalShortBuffer[i] = 0;

    buffer.zero();
}

int Mp3DecoderMiniMp3::getSampleRate() const
{
    if (mp3Info.sample_rate <= 0)
        return 44100;

    return mp3Info.sample_rate;
}

const SamplesBuffer Mp3DecoderMiniMp3::decode(char *inputBuffer, int inputBufferLenght)
{
    array.append(inputBuffer, inputBufferLenght);
    if (array.size() < MINIMUM_SIZE_TO_DECODE)
        return SamplesBuffer::ZERO_BUFFER;

    int totalBytesDecoded = 0;
    int bytesDecoded = 0;
    signed short *out = internalShortBuffer;
    char *in = array.data();
    int totalSamplesDecoded = 0;
    int bytesLeft = array.size() - totalBytesDecoded;
    do {
        bytesDecoded = mp3_decode((void **)mp3Decoder, in, bytesLeft, out, &mp3Info);
        if (bytesDecoded > 0) {
            bytesLeft -= bytesDecoded;
            in += bytesDecoded;
            int samplesDecoded = mp3Info.audio_bytes/2;
            out += samplesDecoded;
            totalSamplesDecoded += samplesDecoded;
            totalBytesDecoded += bytesDecoded;
        }
    } while (bytesDecoded > 0 && bytesLeft > 0);

    array = array.right(array.size() - totalBytesDecoded); // keep just the undecoded bytes to the next call for decode
    if (totalBytesDecoded <= 0)
        return SamplesBuffer::ZERO_BUFFER;

    // +++++++++++++++++++++++++++
    int framesDecoded = totalSamplesDecoded/mp3Info.channels;

    if (mp3Info.channels >= 2)
        buffer.setToStereo();
    else
        buffer.setToMono();

    if (framesDecoded > AUDIO_SAMPLES_BUFFER_MAX_SIZE)
        framesDecoded = AUDIO_SAMPLES_BUFFER_MAX_SIZE;

    buffer.setFrameLenght(framesDecoded);
    int internalIndex = 0;
    for (int i = 0; i < framesDecoded; ++i) {
        for (int c = 0; c < buffer.getChannels(); ++c)
            buffer.set(c, i, (float)internalShortBuffer[internalIndex++] / SHRT_MAX);
    }

    return buffer;
}


