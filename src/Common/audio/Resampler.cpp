#include "Resampler.h"
//#include <cmath>
#include <QDebug>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SimpleResampler::SimpleResampler(){

}

void SimpleResampler::process(const float *in, int inLength, float *out, int outLenght){
    double step = (double)inLength/(double)outLenght;
    double frac = 0;
    int cursor = 0;
    double doubleCursor = 0;
    for (int i = 0; i < outLenght; ++i) {
        cursor = (int)doubleCursor;
        frac = (cursor < inLength-1) ? doubleCursor - cursor : 0;//the last input samples are just copied
        out[i] = in[cursor] * (1.0-frac) + in[cursor+1] * frac;
        doubleCursor += step;
    }
    //return outLenght;
}
