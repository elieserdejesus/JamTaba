#ifndef RESAMPLER_H
#define RESAMPLER_H

class Resampler
{
public:
    int process(const float* in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate);
    Resampler();
    ~Resampler();

private:
    void* libHandler;
};

#endif // RESAMPLER_H
