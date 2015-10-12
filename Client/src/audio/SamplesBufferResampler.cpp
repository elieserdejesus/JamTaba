#include "SamplesBufferResampler.h"
#include <algorithm>
#include <QDebug>

SamplesBufferResampler::SamplesBufferResampler()
    :outBuffer(2, 4096 * 2){

}

SamplesBufferResampler::~SamplesBufferResampler(){
    //qWarning() << "destrutor SamplesBufferResampler";
}

const Audio::SamplesBuffer& SamplesBufferResampler::resample(const Audio::SamplesBuffer &in, int desiredOutLenght){
    outBuffer.zero();
    outBuffer.setFrameLenght(desiredOutLenght * 2);//enough space
    int channels = std::min(in.getChannels(), outBuffer.getChannels());
    for (int c = 0; c < channels; ++c) {
        float* input = in.getSamplesArray(c);
        float* output = outBuffer.getSamplesArray(c);
        resamplers[c].process(input, in.getFrameLenght(), output, desiredOutLenght);
    }
    outBuffer.setFrameLenght(desiredOutLenght);
    return outBuffer;
}
