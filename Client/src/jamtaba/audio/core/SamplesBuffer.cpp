#include "SamplesBuffer.h"
#include <stdexcept>
#include <QDebug>
#include <cmath>
#include <algorithm>

using namespace Audio;
//+++++++++++++++++=

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const SamplesBuffer SamplesBuffer::ZERO_BUFFER(1, 0);

SamplesBuffer::SamplesBuffer(unsigned int channels)
    : channels(channels),
      frameLenght(0)
{
    if(channels == 0){
        throw std::runtime_error(std::string("AudioSamplesBuffer::channels == 0"));
    }
    for (unsigned int c = 0; c < channels; ++c) {
        samples.push_back(std::vector<float>());
    }
}

SamplesBuffer::SamplesBuffer(unsigned int channels, unsigned int frameLenght)
    : channels(channels),
      frameLenght(frameLenght)
{
    if(channels == 0){
        throw std::runtime_error(std::string("AudioSamplesBuffer::channels == 0"));
    }
    for (unsigned int c = 0; c < channels; ++c) {
        samples.push_back(std::vector<float>(frameLenght));
    }
}

SamplesBuffer::SamplesBuffer(const SamplesBuffer& other)
    : channels(other.channels),
      frameLenght(other.frameLenght),
      samples(other.samples)
{
    //qWarning() << "Samples Buffer copy constructor!";
}

SamplesBuffer& SamplesBuffer::operator=(const SamplesBuffer& /*other*/){
    qCritical() << "assignment operator";
    return *this;
}

SamplesBuffer::~SamplesBuffer(){

}


float* SamplesBuffer::getSamplesArray(unsigned int channel) const{
    if( channel > samples.size()){
        channel = 0;
    }
    return const_cast<float*>(&(samples[channel][0]));
}


//void SamplesBuffer::setOffset(int offset){
//    //QMutexLocker locker(&mutex);
//    if(offset > 0){
//        for (unsigned int c = 0; c < channels; ++c) {
//            samples[c] += offset;
//        }
//        this->offset = offset;
//        this->frameLenght -= offset;
//    }
//    else{
//        resetOffset();
//    }

//}

//void SamplesBuffer::resetOffset(){
//    //QMutexLocker locker(&mutex);
//    for (unsigned int c = 0; c < channels; ++c) {
//        samples[c] -= offset;
//    }
//    this->offset = 0;
//}

void SamplesBuffer::applyGain(float gainFactor)
{
    //QMutexLocker locker(&mutex);
    for (unsigned int c = 0; c < channels; ++c) {
        for (unsigned int i = 0; i < frameLenght; ++i) {
            samples[c][i] *= gainFactor;
        }
    }
}

void SamplesBuffer::fadeOut(int fadeFrameLenght, float endGain){
    int lenght = std::min(fadeFrameLenght, (int)frameLenght);
    float gainStep = (1 - endGain)/lenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = 1;
        for (unsigned int s = 0; s < lenght; ++s) {
            samples[c][s] *= gain;
            gain -= gainStep;
        }
    }
}

void SamplesBuffer::fadeIn(int fadeFrameLenght, float beginGain){
    int lenght = std::min(fadeFrameLenght, (int)frameLenght);
    float gainStep = (1 - beginGain)/lenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = beginGain;
        for (unsigned int s = 0; s < lenght; ++s) {
            samples[c][s] *= gain;
            gain += gainStep;
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
    for (unsigned int c = 0; c < channels; ++c) {
        std::fill(samples[c].begin(), samples[c].end(), 0);
    }
}

AudioPeak SamplesBuffer::computePeak() const
{
    //QMutexLocker locker(&mutex);
    float abs;
    float peaks[2];//left and right peaks
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
    return AudioPeak(peaks[0], peaks[1]);
}

//void SamplesBuffer::copyLeftChannelToRight(){
//    if(channels > 1){
//        memcpy(samples[1], samples[0], frameLenght * sizeof(float));
//    }
//}

void SamplesBuffer::add(const SamplesBuffer &buffer, int internalWriteOffset){
    //QMutexLocker locker(&mutex);
    unsigned int framesToProcess = std::min( (int)frameLenght, buffer.getFrameLenght());
//    if(framesToProcess < (unsigned int)buffer.getFrameLenght()){
//        qWarning() << (buffer.getFrameLenght()-framesToProcess) << " samples discarded";
//    }
    if( buffer.channels >= channels){
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s + internalWriteOffset] += buffer.samples[c][s];
            }
        }
    }
    else{//samples is stereo and buffer is mono
        for (unsigned int s = 0; s < framesToProcess; ++s) {
            samples[0][s + internalWriteOffset] += buffer.samples[0][s];
            samples[1][s + internalWriteOffset] += buffer.samples[0][s];
        }
    }
}

void SamplesBuffer::add(unsigned int channel, float *samples, int samplesToAdd){
    if( channel < channels){
        void* dest = &(this->samples[channel][0]);
        memcpy(dest, samples, std::min((int)frameLenght, samplesToAdd) * sizeof(float));
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
    else{
        qWarning() << "channel ("<<channel<<") or sampleIndex ("<<sampleIndex<<") invalid";
    }
}

int SamplesBuffer::getFrameLenght() const{
    //QMutexLocker locker(&mutex);
    return this->frameLenght;
}

void SamplesBuffer::setToMono(){
    if(this->channels != 1){
        this->channels = 1;
    }
}

void SamplesBuffer::setToStereo(){
    if(this->samples.size() < 2){
        int channelsToAdd = 2 - this->samples.size();
        for (int c = 0; c < channelsToAdd; ++c) {
            samples.push_back(std::vector<float>(frameLenght));
        }
    }
    for (unsigned int c = 0; c < samples.size(); ++c) {
        samples[c].resize(frameLenght);
    }
    this->channels = 2;
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

    for (unsigned int c = 0; c < channels; ++c) {
        samples[c].resize(newFrameLenght);
    }
    this->frameLenght = newFrameLenght;
}

void SamplesBuffer::set(const SamplesBuffer &buffer, int bufferChannelOffset, int channelsToCopy){
    int framesToCopy = std::min(buffer.getFrameLenght(), (int)frameLenght);
    int channelsToProcess = std::min(channelsToCopy, std::min(buffer.getChannels(), (int)channels));
    if(channelsToProcess + bufferChannelOffset <= buffer.getChannels()){//avoid invalid channel index
        int bytesToCopy = framesToCopy * sizeof(float);
        for (int c = 0; c < channelsToProcess; ++c) {
            memcpy( (void*)getSamplesArray(c), buffer.getSamplesArray(c + bufferChannelOffset), bytesToCopy);
        }
    }
    else{
        throw new std::runtime_error("wrong channel index ");
    }
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
