#include "AudioDriver.h"
#include "AudioDriverListener.h"
#include <vector>
#include <QDebug>
#include <stdexcept>

AudioSamplesBuffer::AudioSamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT){
    if(channels == 0){
        throw std::runtime_error(std::string("AudioSamplesBuffer::channels == 0"));
    }
    //this->sampleRate = sampleRate;
    this->samples = new float*[channels];
    for (unsigned int c = 0; c < channels; ++c) {
        this->samples[c] = new float[MAX_BUFFERS_LENGHT];
    }
    this->channels = channels;
    this->frameLenght = MAX_BUFFERS_LENGHT;
}

AudioSamplesBuffer::~AudioSamplesBuffer(){
    if(samples != NULL){
        for (unsigned int c = 0; c < channels; ++c) {
            delete samples[c];
            samples[c] = NULL;
        }
        delete samples;
        samples = NULL;
    }
    qDebug() << "\tAudio samples destructor!";
}

void AudioSamplesBuffer::applyGain(float gainFactor)
{
    for (unsigned int c = 0; c < channels; ++c) {
        for (unsigned int i = 0; i < frameLenght; ++i) {
            samples[c][i] *= gainFactor;
        }
    }
}

void AudioSamplesBuffer::zero()
{
    for (unsigned int c = 0; c < channels; ++c) {
       memset(samples[c], 0, frameLenght * sizeof(float));
    }
}

void AudioSamplesBuffer::add(const AudioSamplesBuffer &buffer)
{
    unsigned int framesToProcess = frameLenght < buffer.frameLenght ? frameLenght : buffer.frameLenght;
    if(channels == buffer.channels){
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s] += buffer.samples[c][s];
            }
        }
    }
    else{
        if(!isMono()){//copy every &buffer samples to LR in this buffer
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s] += buffer.samples[0][s];
                samples[1][s] += buffer.samples[0][s];
            }
        }
        else{//this buffer is mono, but the buffer in parameter is not! Mix down the stereo samples in one mono sample value.
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s] += (buffer.samples[0][s] + buffer.samples[1][s]) / 2;
            }
        }
    }
}

void AudioSamplesBuffer::set(const AudioSamplesBuffer &buffer)
{
    unsigned int framesToProcess = frameLenght < buffer.frameLenght ? frameLenght : buffer.frameLenght;
    if(channels == buffer.channels){
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[c][s] = buffer.samples[c][s];
            }
        }
    }
    else{
        if(!isMono()){//copy every &buffer samples to LR in this buffer
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s] = buffer.samples[0][s];
                samples[1][s] = buffer.samples[0][s];
            }
        }
        else{//this buffer is mono, but the buffer in parameter is not! Mix down the stereo samples in one mono sample value.
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                samples[0][s] = (buffer.samples[0][s] + buffer.samples[1][s]) / 2;
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
    inputBuffer = new AudioSamplesBuffer(newMaxInputChannels, buffersLenght);

    if (outputBuffer != NULL){
        delete outputBuffer;
    }

    maxOutputChannels = newMaxOutputChannels;
    outputBuffer = new AudioSamplesBuffer(newMaxOutputChannels, buffersLenght);
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
    qDebug() << "[portaudio driver properties changed] " <<
                "\n\tthis->inputDeviceIndex: " << this->inputDeviceIndex <<
                "\n\tthis->outputDeviceIndex: " << this->outputDeviceIndex <<
                "\n\tthis->firstInputIndex: " << this->firstInputIndex <<
                "\n\tthis->inputChannels: " << this->inputChannels <<
                "\n\tthis->firstOutputIndex: " << this->firstOutputIndex <<
                "\n\tthis->outputChannels: " << this->outputChannels <<
                "\n\tthis->sampleRate: " << this->sampleRate <<
                "\n\tthis->bufferSize: " << this->bufferSize;
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



void AbstractAudioDriver::fireDriverCallback(AudioSamplesBuffer& in, AudioSamplesBuffer& out){
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->processCallBack(in, out);
    }
}



