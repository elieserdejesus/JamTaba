#ifndef SAMPLESBUFFERRESAMPLER_H
#define SAMPLESBUFFERRESAMPLER_H

#include "Resampler.h"
#include "core/SamplesBuffer.h"

class SamplesBufferResampler
{

public:
    SamplesBufferResampler();
    ~SamplesBufferResampler();
    const audio::SamplesBuffer &resample(const audio::SamplesBuffer &in, int desiredOutLenght);

private:
    audio::SamplesBuffer outBuffer;
    SimpleResampler resamplers[2];
};

#endif // SAMPLESBUFFERRESAMPLER_H
