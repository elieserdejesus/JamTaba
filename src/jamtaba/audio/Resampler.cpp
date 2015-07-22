#include "Resampler.h"
//#include <cmath>
#include <QDebug>

#include "libresample.h"

Resampler::Resampler(){
    libHandler = resample_open(0, 44100.0/192000.0, 192000.0/44100.0);
    //qWarning() << "criando resampler";
}

Resampler::~Resampler(){
    resample_close(libHandler);
}

int Resampler::process(const float *in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate){
    double factor = (double)outSampleRate/(double)inSampleRate;
    int inOffset = 0;
    int totalResampled = 0;
    int resampled = 0;
    do{
        int inBlock = inLength - inOffset;
        resampled = resample_process(libHandler,
                                           factor,
                                           (float*)(in + inOffset), //input
                                           inBlock,
                                           lastInputChunk,//last flag
                                            &inOffset, //inBufferUsed
                                           out + totalResampled, //out buffer
                                           outLenght - totalResampled );//outBufferLen
        if(resampled >= 0){
            totalResampled += resampled;
        }
    }
    while(resampled >= 0 && totalResampled < outLenght);
    return totalResampled;
}
