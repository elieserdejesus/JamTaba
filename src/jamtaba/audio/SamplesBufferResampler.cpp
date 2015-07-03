#include "SamplesBufferResampler.h"

SamplesBufferResampler::SamplesBufferResampler()
    :outBuffer(2, 4096 * 2){

}

SamplesBufferResampler::~SamplesBufferResampler(){

}

const Audio::SamplesBuffer& SamplesBufferResampler::resample(
        Audio::SamplesBuffer &in, int maxOutLenght, int originalSampleRate, int finalSampleRate)
{
    int outLenght = 0;
    outBuffer.zero();
    for (int c = 0; c < 2; ++c) {
        float* input = in.getSamplesArray(c);
        float* output = outBuffer.getSamplesArray(c);
        outLenght = Resampler::process(input, in.getFrameLenght(), originalSampleRate, output, maxOutLenght, finalSampleRate);
    }
    outBuffer.setFrameLenght(outLenght);
    return outBuffer;
}
