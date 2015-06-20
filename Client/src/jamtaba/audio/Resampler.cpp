#include "Resampler.h"

#include "libresample/include/libresample.h"

Resampler::Resampler(double originalSampleRate, double finalSampleRate)
    :factor(finalSampleRate/originalSampleRate){
    libresampleHandle = resample_open(0, 44100.0/192000, 192000/44100.0);
}

Resampler::~Resampler(){
    resample_close(libresampleHandle);
}

int Resampler::process(float* in, int inLength, float* out, int outLength){
    int lastSize = 0;
    int inOffset = 0;
    bool finished = false;
    do{
        int size = resample_process(libresampleHandle, factor, in, inLength, 1, &inOffset, out, outLength);
        int lastInOffset = inOffset;
        lastSize = size;
        finished = size == lastSize && inOffset == lastInOffset;//no input consumed and no output generated
    }
    while(!finished);
    return lastSize;
}


