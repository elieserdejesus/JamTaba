#ifndef RESAMPLER_H
#define RESAMPLER_H

class Resampler
{
public:
    //static int process(const float* in, int inLength, int inSampleRate, float *out, int desiredOutLength, int outSampleRate);
    static int process(const float *in, int inLength, float *out, int desiredOutLength);
};

#endif // RESAMPLER_H
