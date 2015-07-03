#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <vector>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>

using namespace Audio;




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
            it = this->listeners.erase(it);
        }
        else{
            ++it;
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
    inputBuffer = new SamplesBuffer(newMaxInputChannels);

    if (outputBuffer != NULL){
        delete outputBuffer;
    }

    maxOutputChannels = newMaxOutputChannels;
    outputBuffer = new SamplesBuffer(newMaxOutputChannels);
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
    for (; it != listeners.end(); ++it){
        (*it)->driverStarted();
    }
}

void AbstractAudioDriver::fireDriverStopped() const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); ++it){
        (*it)->driverStopped();
    }
}

void AbstractAudioDriver::fireDriverException(const char* msg) const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); ++it){
        (*it)->driverException(msg);
    }
}



void AbstractAudioDriver::fireDriverCallback(SamplesBuffer& in, SamplesBuffer& out) const{
    std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
    for (; it != listeners.end(); ++it){
        (*it)->processCallBack(in, out);
    }
}



