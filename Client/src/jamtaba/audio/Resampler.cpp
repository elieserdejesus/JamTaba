#include "Resampler.h"
//#include <cmath>
#include <QDebug>

#include "libresample.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int ResamplerTest::process(const float *in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate){
    double factor = (double)outSampleRate/(double)inSampleRate;

    //double perda = (inLength * factor) -

    int finalSize = outLenght;// std::min(outLenght, (int)(inLength * factor));
    double step = (double)inSampleRate/(double)outSampleRate;
    for (int i = 0; i < finalSize; ++i) {
        out[i] = in[ (int)(i * step) ];
    }
    return finalSize;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ResamplerLibResampler::ResamplerLibResampler()
{
    libHandler = resample_open(1, 44100.0/192000.0, 192000.0/44100.0);
    //qWarning() << "criando resampler";
}

ResamplerLibResampler::~ResamplerLibResampler(){
    resample_close(libHandler);
}

int ResamplerLibResampler::process(const float *in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate){

    double factor = (double)outSampleRate/(double)inSampleRate;


    int consumed = 0;
    int totalResampled = 0;
    int resampled = 0;
    int inOffset = 0;
    while(true){
        int inBlock = inLength - inOffset;
        resampled = resample_process(libHandler,
                                           factor,
                                           (float*)(in + inOffset), //input
                                           inBlock,
                                           lastInputChunk,//last flag
                                            &consumed, //inBufferUsed
                                           out + totalResampled, //out buffer
                                           outLenght - totalResampled );//outBufferLen
        inOffset += consumed;
        if(resampled >= 0){
            totalResampled += resampled;
        }

//        if(totalResampled >= outLenght || (resampled == 0 && lastInputChunk)){
//            break;
//        }
        if (resampled < 0 || (resampled == 0 && inOffset == inLength) || totalResampled >= outLenght){
           break;
        }
    }

    //while(resampled >= 0 && inOffset < inLength && totalResampled < outLenght);
    return totalResampled;
}
