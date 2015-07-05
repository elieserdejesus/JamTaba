#include "SamplesBufferResampler.h"

SamplesBufferResampler::SamplesBufferResampler()
    :outBuffer(2, 4096 * 2){

}

SamplesBufferResampler::~SamplesBufferResampler(){

}

const Audio::SamplesBuffer& SamplesBufferResampler::resample(
        Audio::SamplesBuffer &in, int desiredOutLenght)
{
    int lenght = 0;
    outBuffer.zero();
    for (int c = 0; c < 2; ++c) {
        float* input = in.getSamplesArray(c);
        float* output = outBuffer.getSamplesArray(c);
        lenght = Resampler::process(input, in.getFrameLenght(), output, desiredOutLenght);
    }
    outBuffer.setFrameLenght(lenght);
    return outBuffer;
}
