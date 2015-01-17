#include "AudioDriver.h"
#include "AudioDriverListener.h"
#include <vector>
#include <QDebug>

AbstractAudioDriver::AbstractAudioDriver(){
    //qDebug() << "AbstractAudioDriver constructor...";
    inputBuffers = 0;//non interleaved buffers
    outputBuffers = 0;
    inputMasks = outputMasks = 0;
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


void AbstractAudioDriver::recreateInputBuffers(const int buffersLenght, const int newMaxInputChannels){
    if (inputBuffers != NULL){
        for (int i = 0; i < maxInputChannels; i++){
            delete inputBuffers[i];
        }
        delete inputBuffers;
    }
    if(inputMasks != NULL){
        delete inputMasks;
    }

    //recreate
    maxInputChannels = newMaxInputChannels;
    inputBuffers = new float*[newMaxInputChannels];
    for (int i = 0; i < newMaxInputChannels; i++){
        inputBuffers[i] = new float[buffersLenght];
    }

    inputMasks = new float[newMaxInputChannels];
    for (int i = 0; i < newMaxInputChannels; i++){
        inputMasks[i] = (i >= firstInputIndex && i <= firstInputIndex + inputChannels -1) ? 1 : 0;
    }
}

void AbstractAudioDriver::recreateOutputBuffers(const int buffersLenght, const int newMaxOutputChannels){
    if (outputBuffers != NULL){
        for (int i = 0; i < maxOutputChannels; i++){
            delete outputBuffers[i];
        }
        delete outputBuffers;
    }
    if(outputMasks != NULL){
        delete outputMasks;
    }
    maxOutputChannels = newMaxOutputChannels;
    outputBuffers = new float*[newMaxOutputChannels];
    for (int i = 0; i < newMaxOutputChannels; i++){
        outputBuffers[i] = new float[buffersLenght];
    }

    //recreate
    outputMasks = new float[newMaxOutputChannels];
    for (int i = 0; i < newMaxOutputChannels; i++){
        outputMasks[i] = (i >= firstOutputIndex && i <= firstOutputIndex + outputChannels -1) ? 1 : 0;
    }
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

void AbstractAudioDriver::fireDriverCallback(float** in, float** out, const int samplesToProcess){
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); it++){
        (*it)->processCallBack(in, out, samplesToProcess);
    }
}



