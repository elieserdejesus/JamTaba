#include "Resampler.h"
#include <cmath>
#include <QDebug>

//#include "libresample/include/libresample.h"

//void* Resampler::libresampleHandle = nullptr;

//Resampler::Resampler(double originalSampleRate, double finalSampleRate)
//    :factor(finalSampleRate/originalSampleRate){
//    openLibHandle();
//}

//Resampler::Resampler()
//    :factor(1){
//    openLibHandle();
//}

//void Resampler::openLibHandle(){
//    if(!libresampleHandle){
//        libresampleHandle = resample_open(0, 44100.0/192000.0, 192000.0/44100.0);
//    }
//}

//void Resampler::releaseSharedLibHandler(){
//    //resample_close(libresampleHandle);
//}

//void Resampler::setSampleRates(double originalSampleRate, double finalSampleRate){
//    factor = finalSampleRate/originalSampleRate;
//}

//Resampler::~Resampler(){

//}

int Resampler::process(float *in, int inLength, int inSampleRate, float *out, int outLength, int outSampleRate){
    if (inSampleRate == outSampleRate) {
        //copy in to out
        return inLength;
    }

    int newLength = std::ceil( ((float) inLength / inSampleRate * outSampleRate));
    if(newLength > outLength){
        qWarning() << "newLength > outLength" << newLength << " > " << outLength;
    }
    float lengthMultiplier = (float) newLength / inLength;
    //short[] interpolatedSamples = new short[newLength];

    // interpolate the value by the linear equation y=mx+c
    for (int i = 0; i < newLength; i++) {

        // get the nearest positions for the interpolated point
        float currentPosition = i / lengthMultiplier;
        int nearestLeftPosition = (int) currentPosition;
        int nearestRightPosition = nearestLeftPosition + 1;
        if (nearestRightPosition >= inLength) {
            nearestRightPosition = inLength - 1;
        }

        float slope = in[nearestRightPosition] - in[nearestLeftPosition];     // delta x is 1
        float positionFromLeft = currentPosition - nearestLeftPosition;

        out[i] = slope * positionFromLeft + in[nearestLeftPosition];      // y=mx+c
    }
    return newLength;
}

//int Resampler::process(float* in, int inLength, float* out, int outLength){



//    int lastSize = 0;
//    int inOffset = 0;
//    bool finished = false;
//    do{
//        int size = resample_process(libresampleHandle, factor, in, inLength, 0, &inOffset, out, outLength);
//        int lastInOffset = inOffset;
//        lastSize = size;
//        finished = size == lastSize && inOffset == lastInOffset;//no input consumed and no output generated
//    }
//    while(!finished);
//    return lastSize;
//}


