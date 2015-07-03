#ifndef RESAMPLER_H
#define RESAMPLER_H

class Resampler
{
public:
//    Resampler(double originalSampleRate, double finalSampleRate);
//    Resampler();
//    ~Resampler();
//    int process(float* in, int inLength, float* out, int outLength);
    static int process(float* in, int inLength, int inSampleRate, float* out, int outLength, int outSampleRate);

    //static void releaseSharedLibHandler();

    //void setSampleRates( double originalSampleRate, double finalSampleRate);

private:
    //double factor;
    //static void* libresampleHandle;
    //static void openLibHandle();
};

#endif // RESAMPLER_H
