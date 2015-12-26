#ifndef RESAMPLER_H
#define RESAMPLER_H

class ResamplerTest
{
public:
    explicit ResamplerTest();
    void process(const float *in, int inLength, float *out, int outLenght);
private:
};

#endif // RESAMPLER_H
