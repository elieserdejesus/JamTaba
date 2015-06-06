#ifndef RESAMPLER_H
#define RESAMPLER_H


class Resampler
{
public:
    Resampler(double originalSampleRate, double finalSampleRate);

    ~Resampler();

    int process(float* in, int inLength, float* out, int outLength);

private:
    double factor;
    void* libresampleHandle;
};

#endif // RESAMPLER_H
