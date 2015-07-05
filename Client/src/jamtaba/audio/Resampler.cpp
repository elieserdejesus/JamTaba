#include "Resampler.h"
#include <cmath>
#include <QDebug>


int Resampler::process(const float *in, int inLength, float *out, int desiredOutLength){
    if (inLength == desiredOutLength) {
        //copy in to out
        return inLength;
    }

    float lengthMultiplier = (float) desiredOutLength / inLength;

    // interpolate the value by the linear equation y=mx+c
    for (int i = 0; i < desiredOutLength; i++) {
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
    return desiredOutLength;
}

/*
int Resampler::process(const float *in, int inLength, int inSampleRate, float *out, int desiredOutLength, int outSampleRate){
    if (inSampleRate == outSampleRate) {
        //copy in to out
        return inLength;
    }

    int newLength = std::round( ((float) inLength / inSampleRate * outSampleRate));
    if(newLength != desiredOutLength){
        qWarning() << "newLength != desiredOutLength" << newLength << " != " << desiredOutLength;
    }
    float lengthMultiplier = (float) newLength / inLength;

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

*/
