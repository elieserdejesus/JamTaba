#ifndef SAMPLESBUFFERRESAMPLER_H
#define SAMPLESBUFFERRESAMPLER_H

#include "Resampler.h"
#include "core/SamplesBuffer.h"

class SamplesBufferResampler
{
public:
    SamplesBufferResampler();
    ~SamplesBufferResampler();
    const Audio::SamplesBuffer& resample(Audio::SamplesBuffer& in, int desiredOutLenght);
private:
    Audio::SamplesBuffer outBuffer;
};

#endif // SAMPLESBUFFERRESAMPLER_H
