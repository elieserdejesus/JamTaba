#include "SamplesBufferResampler.h"

SamplesBufferResampler::SamplesBufferResampler()
    :outBuffer(2, 4096 * 2){

}

SamplesBufferResampler::~SamplesBufferResampler(){

}

const Audio::SamplesBuffer& SamplesBufferResampler::resample(Audio::SamplesBuffer &in, int inSampleRate, int desiredOutLenght, int targetSampleRate)
{
    int lenght = 0;
    outBuffer.zero();
    outBuffer.setFrameLenght(desiredOutLenght * 2);//enough space
    for (int c = 0; c < 2; ++c) {
        float* input = in.getSamplesArray(c);
        float* output = outBuffer.getSamplesArray(c);
        lenght = resamplers[c].process(input, in.getFrameLenght(), inSampleRate, output, desiredOutLenght, targetSampleRate);
    }
    outBuffer.setFrameLenght(lenght);
    return outBuffer;
}
