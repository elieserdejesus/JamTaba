#include "streambuffer.h"
#include "../audio/core/AudioDriver.h"

#include <QDebug>

using namespace Audio;

StreamBuffer::StreamBuffer(int channels, int capacity)
    :channels(channels), capacity(capacity), writeIndex(0), readIndex(0), available(0){
    samples = new float*[channels];
    for (int c = 0; c < channels; ++c) {
        samples[c] = new float[capacity];
        for (int x = 0; x < capacity; ++x) {
            samples[c][x] = 0;
        }
    }
}

StreamBuffer::~StreamBuffer(){
    for (int c = 0; c < channels; ++c) {
        delete [] samples[c];
    }
    delete [] samples;
}

void StreamBuffer::add(float leftSample, float rightSample){
    if(channels != 2){
        qCritical() << "channels != 2";
    }
    samples[0][writeIndex] = leftSample;
    samples[1][writeIndex] = rightSample;
    writeIndex = (writeIndex + 1) % capacity;
    available++;
}

void StreamBuffer::add(float monoSample){
    if(channels != 1){
        qCritical() << "channels != 1, buffer is not mono";
    }
    samples[0][writeIndex] = monoSample;
    writeIndex = (writeIndex + 1) % capacity;
    available++;
}

void StreamBuffer::fillBuffer(Audio::SamplesBuffer& buffer){
    int samplesToCopy = std::min(buffer.getFrameLenght(), available);
    if( channels <= buffer.getChannels()){
        float sampleValue = 0;
        for (unsigned int s = 0; s < samplesToCopy; ++s) {
            for (unsigned int c = 0; c < channels; ++c) {
                sampleValue = samples[c][readIndex];
                buffer.set(c, s, sampleValue);
            }
            readIndex = (readIndex + 1) % capacity;
            available--;
        }
    }
    else{
        qCritical() << "não copiou nada!";
        //samples is stereo and buffer is mono
        //            for (unsigned int s = 0; s < framesToProcess; ++s) {
        //                samples[0][s ] += buffer.samples[0][s];
        //                samples[1][s] += buffer.samples[0][s];
        //            }
    }
}

