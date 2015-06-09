#include "AudioDriver.h"
#include <vector>
#include <QDebug>
#include <stdexcept>
#include <cmath>
#include <QMutexLocker>

using namespace Audio;

//int AudioSamplesBuffer:: lastID = 0;

//SamplesBuffer::SamplesBuffer(float **samples, unsigned int channels, unsigned int samplesCount)
//    :   channels(channels),
//        frameLenght(samplesCount),
//        maxFrameLenght(samplesCount),
//        offset(0)
//{
//    this->samples = samples;
//    this->peaks[0] = this->peaks[1] = 0;
//}

SamplesBuffer::SamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT)
    : channels(channels),
      frameLenght(MAX_BUFFERS_LENGHT),
      maxFrameLenght(MAX_BUFFERS_LENGHT),
      offset(0)

{
    if(channels == 0){
        throw std::runtime_error(std::string("AudioSamplesBuffer::channels == 0"));
    }

    //this->sampleRate = sampleRate;
    this->samples = new float*[channels];
    for (unsigned int c = 0; c < channels; ++c) {
        this->samples[c] = new float[MAX_BUFFERS_LENGHT];
        for (unsigned int s = 0; s < MAX_BUFFERS_LENGHT; ++s) {
            this->samples[c][s] = 0;
        }
    }

    this->peaks[0] = this->peaks[1] = 0;
    //this->ID = lastID++;
    //this->isClone = false;
}

SamplesBuffer::~SamplesBuffer(){
    if(samples != nullptr){
        for (unsigned int c = 0; c < channels; ++c) {
            delete [] samples[c];
            samples[c] = nullptr;
        }
        delete [] samples;
        samples = nullptr;
    }
    //qDebug() << "\tAudio samples destructor ID:" << ID;
}


void SamplesBuffer::setOffset(int offset){
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

void SamplesBuffer::add(const SamplesBuffer &buffer)
{
    //QMutexLocker locker(&mutex);
    unsigned int framesToProcess = frameLenght < buffer.frameLenght ? frameLenght : buffer.frameLenght;
    if( buffer.channels >= channels){
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s] += buffer.samples[c][s];
            }
        }
    }
    else{//samples is stereo and buffer is mono
        for (unsigned int s = 0; s < framesToProcess; ++s) {
            samples[0][s ] += buffer.samples[0][s];
            samples[1][s] += buffer.samples[0][s];
        }
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

//+++++++++++++++++++++++++++++++++++++++++++
AbstractAudioDriver::AbstractAudioDriver(){
    //qDebug() << "AbstractAudioDriver constructor...";
    inputBuffer = 0;//non interleaved buffers
    outputBuffer = 0;
    inputChannels = 0;//total of selected input channels
    outputChannels = 0;//total of selected output channels (2 channels by default)
    maxInputChannels = maxOutputChannels = 0;
    inputDeviceIndex = 0;//index of selected device index. In ASIO the inputDeviceIndex and outputDeviceIndex are equal.
    outputDeviceIndex = 0;
    firstInputIndex = 0;
    firstOutputIndex = 0;
    sampleRate = 44100;
    bufferSize = 128;
}

void AbstractAudioDriver::addListener(AudioDriverListener& l){
    this->listeners.push_back(&l);
}

void AbstractAudioDriver::removeListener(AudioDriverListener& l){
    std::vector< AudioDriverListener*>::iterator it = this->listeners.begin();
    while (it != this->listeners.end()){
        if ((*it) == &l){
            this->listeners.erase(it);
        }
        else{
            it++;
        }
    }
}


void AbstractAudioDriver::recreateBuffers(const int buffersLenght, const int newMaxInputChannels, const int newMaxOutputChannels)
{
    if (inputBuffer != NULL){
        delete inputBuffer;
    }

    //recreate
    maxInputChannels = newMaxInputChannels;
    inputBuffer = new SamplesBuffer(newMaxInputChannels, buffersLenght);

    if (outputBuffer != NULL){
        delete outputBuffer;
    }

    maxOutputChannels = newMaxOutputChannels;
    outputBuffer = new SamplesBuffer(newMaxOutputChannels, buffersLenght);
}

void AbstractAudioDriver::setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    setProperties(deviceIndex, deviceIndex, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
}

void AbstractAudioDriver::setProperties(int inputDeviceIndex, int outputDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    stop();
    this->inputDeviceIndex = inputDeviceIndex;
    this->outputDeviceIndex = outputDeviceIndex;
    this->firstInputIndex = firstIn;
    this->inputChannels = (lastIn - firstIn) + 1;
    this->firstOutputIndex = firstOut;
    this->outputChannels = (lastOut - firstOut) + 1;
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
//    qDebug() << "[portaudio driver properties changed] " <<
//                "\n\tthis->inputDeviceIndex: " << this->inputDeviceIndex <<
//                "\n\tthis->outputDeviceIndex: " << this->outputDeviceIndex <<
//                "\n\tthis->firstInputIndex: " << this->firstInputIndex <<
//                "\n\tthis->inputChannels: " << this->inputChannels <<
//                "\n\tthis->firstOutputIndex: " << this->firstOutputIndex <<
//                "\n\tthis->outputChannels: " << this->outputChannels <<
//                "\n\tthis->sampleRate: " << this->sampleRate <<
//                "\n\tthis->bufferSize: " << this->bufferSize;
}

void AbstractAudioDriver::fireDriverStarted() const{
    std::vector< AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->driverStarted();
    }
}

void AbstractAudioDriver::fireDriverStopped() const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->driverStopped();
    }
}

void AbstractAudioDriver::fireDriverException(const char* msg) const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->driverException(msg);
    }
}



void AbstractAudioDriver::fireDriverCallback(SamplesBuffer& in, SamplesBuffer& out) const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->processCallBack(in, out);
    }
}



