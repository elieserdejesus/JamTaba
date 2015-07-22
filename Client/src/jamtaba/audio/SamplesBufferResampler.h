#ifndef SAMPLESBUFFERRESAMPLER_H
#define SAMPLESBUFFERRESAMPLER_H

#include "Resampler.h"
#include "core/SamplesBuffer.h"

class SamplesBufferResampler
{
public:
    SamplesBufferResampler();
    ~SamplesBufferResampler();
    //const Audio::SamplesBuffer& resample(Audio::SamplesBuffer& in, int desiredOutLenght);
    const Audio::SamplesBuffer& resample(Audio::SamplesBuffer& in, int inSampleRate, bool lastInChunk, int desiredOutLenght, int targetSampleRate);
private:
    Audio::SamplesBuffer outBuffer;
    Resampler resamplers[2];
};

#endif // SAMPLESBUFFERRESAMPLER_H
