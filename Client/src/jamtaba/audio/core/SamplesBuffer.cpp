#include "SamplesBuffer.h"
#include <stdexcept>
#include <QDebug>

using namespace Audio;

const SamplesBuffer SamplesBuffer::ZERO_BUFFER(1, 0);

SamplesBuffer::SamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT)
    : channels(channels),
      frameLenght(MAX_BUFFERS_LENGHT),
      maxFrameLenght(MAX_BUFFERS_LENGHT),
      offset(0)
      //deletado(false)

{
    if(channels == 0){
        throw std::runtime_error(std::string("AudioSamplesBuffer::channels == 0"));
    }
    this->peaks[0] = this->peaks[1] = 0;
    samples = new float*[channels];
    for (unsigned int c = 0; c < channels; ++c) {
        samples[c] = new float[MAX_BUFFERS_LENGHT];
    }
}

SamplesBuffer::SamplesBuffer(const SamplesBuffer& other)
    : channels(other.channels),
      frameLenght(other.frameLenght),
      maxFrameLenght(other.maxFrameLenght),
      offset(other.offset)
{
    qCritical() << "copy constructor!";
}

SamplesBuffer& SamplesBuffer::operator=(const SamplesBuffer& /*other*/){
    qCritical() << "assignment operator";
    return *this;
}

SamplesBuffer::~SamplesBuffer(){
    //QMutexLocker locker(&mutex);
    //qDebug() << "desctrutor SamplesBuffer";
    if(samples){
        resetOffset();
        for (unsigned int c = 0; c < channels; ++c) {
            delete [] samples[c];
            //samples[c] = nullptr;
        }
        delete [] samples;
        samples = nullptr;
    }
    //qDebug() << "\tAudio samples destructor ID:" << ID;
}




void SamplesBuffer::setOffset(int offset){
    //QMutexLocker locker(&mutex);
    if(offset > 0){
        for (unsigned int c = 0; c < channels; ++c) {
            samples[c] += offset;
        }
        this->offset = offset;
        this->frameLenght -= offset;
    }
    else{
        resetOffset();
    }

}

void SamplesBuffer::resetOffset(){
    //QMutexLocker locker(&mutex);
    for (unsigned int c = 0; c < channels; ++c) {
        samples[c] -= offset;
    }
    this->offset = 0;
}

void SamplesBuffer::applyGain(float gainFactor)
{
    //QMutexLocker locker(&mutex);
    for (unsigned int c = 0; c < channels; ++c) {
        for (unsigned int i = 0; i < frameLenght; ++i) {
            samples[c][i] *= gainFactor;
        }
    }
}

void SamplesBuffer::fade(float beginGain, float endGain){
    //QMutexLocker locker(&mutex);
    float gainStep = (endGain - beginGain)/frameLenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = beginGain;
        for (unsigned int s = 0; s < frameLenght; ++s) {
            samples[c][s] *= gain;
            gain += gainStep;
        }
    }
}

void SamplesBuffer::applyGain(float gainFactor, float leftGain, float rightGain)
{
    //QMutexLocker locker(&mutex);
    if(!isMono()){
        float finalLeftGain = gainFactor * leftGain;
        float finalRightGain = gainFactor * rightGain;
        for (unsigned int i = 0; i < frameLenght; ++i) {
            samples[0][i] *= finalLeftGain;
            samples[1][i] *= finalRightGain;
        }
    }
    else{
        applyGain(gainFactor);
    }
}


void SamplesBuffer::zero()
{
    //QMutexLocker locker(&mutex);
    for (unsigned int c = 0; c < channels; ++c) {
        void* dest = samples[c];
        memset(dest, 0, maxFrameLenght * sizeof(float));
    }
}

const float *SamplesBuffer::getPeaks() const
{
    //QMutexLocker locker(&mutex);
    float abs;
    for (unsigned int c = 0; c < channels; ++c) {
        float maxPeak = 0;
        for (unsigned int i = 0; i < frameLenght; ++i) {
            abs = fabs(samples[c][i]);
            if( abs > maxPeak){
                maxPeak = abs;
            }
            peaks[c] = maxPeak;
        }
    }
    if(isMono()){
        peaks[1] = peaks[0];
    }
    return peaks;
}

void SamplesBuffer::copyLeftChannelToRight(){
    if(channels > 1){
        memcpy(samples[1], samples[0], frameLenght * sizeof(float));
    }
}

void SamplesBuffer::add(const SamplesBuffer &buffer, int offset){
    //QMutexLocker locker(&mutex);
    unsigned int framesToProcess = frameLenght < buffer.frameLenght ? frameLenght : buffer.frameLenght;
    if( buffer.channels >= channels){
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s + offset] += buffer.samples[c][s];
            }
        }
    }
    else{//samples is stereo and buffer is mono
        for (unsigned int s = 0; s < framesToProcess; ++s) {
            samples[0][s + offset] += buffer.samples[0][s];
            samples[1][s + offset] += buffer.samples[0][s];
        }
    }
}

void SamplesBuffer::add(int channel, float *samples, int samplesToAdd){
    if(channel >= 0 && channel < channels){
        memcpy( this->samples[channel], samples, std::min((int)frameLenght, samplesToAdd) * sizeof(float));
    }
    else{
        qWarning() << "wrong channel " << channel;
    }
}

void SamplesBuffer::add( int channel,  int sampleIndex, float sampleValue){
    //QMutexLocker locker(&mutex);
    if(channelIsValid(channel) && sampleIndexIsValid(sampleIndex)){

        samples[channel][sampleIndex] += sampleValue;
    }
    else{
        qWarning() << "channel ("<<channel<<") or sampleIndex ("<<sampleIndex<<") invalid";
    }
}

void SamplesBuffer::set(int channel, int sampleIndex, float sampleValue){
    //QMutexLocker locker(&mutex);
    if(channelIsValid(channel) && sampleIndexIsValid(sampleIndex)){

        samples[channel][sampleIndex] = sampleValue;
    }
//    else{
//        qWarning() << "channel ("<<channel<<") or sampleIndex ("<<sampleIndex<<") invalid";
//    }
}

int SamplesBuffer::getFrameLenght() const{
    //QMutexLocker locker(&mutex);
    return this->frameLenght;
}

void SamplesBuffer::set(const SamplesBuffer &buffer){
    set(buffer, 0, buffer.getFrameLenght(), 0);
}

float SamplesBuffer::get(int channel, int sampleIndex) const{
    //QMutexLocker locker(&mutex);
    if(!channelIsValid(channel) || !sampleIndexIsValid(sampleIndex)){
        return 0;
    }
    return samples[channel][sampleIndex ];

}

void SamplesBuffer::setFrameLenght(unsigned int newFrameLenght){
    //QMutexLocker locker(&mutex);
    if(newFrameLenght == frameLenght){
        return;
    }
    if(newFrameLenght > maxFrameLenght){
        qWarning() << "newFrameLenght > maxFrameLenght (" << newFrameLenght << " > " << maxFrameLenght << ")  frameLenght:" << frameLenght;
        newFrameLenght = maxFrameLenght;
    }
    //if(newFrameLenght > this->frameLenght){
        //zero extra samples
        //zero(frameLenght, maxFrameLenght-1);
    //}
    this->frameLenght = newFrameLenght;
}

void SamplesBuffer::set(const SamplesBuffer& buffer, unsigned int bufferOffset, unsigned int samplesToCopy, unsigned int internalOffset){

    //QMutexLocker locker(&mutex);
    //qDebug() << "set";
    unsigned int framesToProcess = std::min(samplesToCopy, buffer.getFrameLenght() - bufferOffset);
    if(framesToProcess > buffer.frameLenght){//não processa mais samples do que a quantidade existente em buffer
        framesToProcess = buffer.frameLenght;
    }
    if((int)(internalOffset + framesToProcess)  > this->getFrameLenght()){
        framesToProcess = (internalOffset + framesToProcess) - this->getFrameLenght();
    }

    if(channels == buffer.channels){//channels number are equal
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s + internalOffset] = buffer.samples[c][s + bufferOffset];
            }
        }
    }
    else{//different number of channels
        if(!isMono()){//copy every &buffer samples to LR in this buffer
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s + internalOffset] = buffer.samples[0][s + bufferOffset];
                samples[1][s + internalOffset] = buffer.samples[0][s + bufferOffset];
            }
        }
        else{//this buffer is mono, but the buffer in parameter is not! Mix down the stereo samples in one mono sample value.
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s + internalOffset] = (buffer.samples[0][s + bufferOffset] + buffer.samples[1][s + bufferOffset]) / 2;
            }
        }
    }

}
