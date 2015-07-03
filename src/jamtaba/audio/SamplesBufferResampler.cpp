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
        float* resamplerIn = in.getSamplesArray()[c];
        float* resamplerOut = outBuffer.getSamplesArray()[c];
        outLenght = Resampler::process(resamplerIn, in.getFrameLenght(), originalSampleRate, resamplerOut, maxOutLenght, finalSampleRate);
    }
    outBuffer.setFrameLenght(outLenght);
    return outBuffer;
}
