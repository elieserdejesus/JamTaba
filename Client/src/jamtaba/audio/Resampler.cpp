#include "Resampler.h"
//#include <cmath>
#include <QDebug>

#include "libresample.h"

Resampler::Resampler(){
    libHandler = resample_open(0, 44100.0/192000.0, 192000.0/44100.0);
    qWarning() << "criando resampler";
}

Resampler::~Resampler(){
    resample_close(libHandler);
}

int Resampler::process(const float *in, int inLength, int inSampleRate, float *out, int outLenght, int outSampleRate){
    double factor = (double)outSampleRate/(double)inSampleRate;
    int inOffset = 0;
    int totalResampled = 0;
    while(inOffset < inLength){
        totalResampled += resample_process(libHandler, factor, (float*)(in + inOffset), inLength, 0, &inOffset, out, outLenght);
    }
    return totalResampled;
}
