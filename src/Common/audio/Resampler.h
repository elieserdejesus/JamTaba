#ifndef RESAMPLER_H
#define RESAMPLER_H

class SimpleResampler
{
public:
    explicit SimpleResampler();
    void process(const float *in, int inLength, float *out, int outLenght);
private:
};

#endif // RESAMPLER_H
