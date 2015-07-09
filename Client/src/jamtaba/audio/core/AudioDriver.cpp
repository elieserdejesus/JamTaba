#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <vector>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>

using namespace Audio;




//+++++++++++++++++++++++++++++++++++++++++++
AudioDriver::AudioDriver(AudioDriverListener *audioDriverListener)
    :
    maxInputChannels(0),
    maxOutputChannels(0),
    inputChannels(0),//total of selected input channels
    outputChannels(0),//total of selected output channels (2 channels by default)
    inputDeviceIndex(0),//index of selected device index. In ASIO the inputDeviceIndex and outputDeviceIndex are equal.
    outputDeviceIndex(0),
    firstInputIndex(0),
    firstOutputIndex(0),
    sampleRate(44100),
    bufferSize(128),
    inputBuffer(2),
    outputBuffer(2),
    audioDriverListener(audioDriverListener)
{

}

void AudioDriver::recreateBuffers(const int newMaxInputChannels, const int newMaxOutputChannels){
//    if (inputBuffer != NULL){
//        delete inputBuffer;
//    }

    //recreate
    maxInputChannels = newMaxInputChannels;
    //inputBuffer = new SamplesBuffer(newMaxInputChannels);

//    if (outputBuffer != NULL){
//        delete outputBuffer;
//    }

    maxOutputChannels = newMaxOutputChannels;
    //outputBuffer = new SamplesBuffer(newMaxOutputChannels);
}

void AudioDriver::setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    setProperties(deviceIndex, deviceIndex, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
}

void AudioDriver::setProperties(int inputDeviceIndex, int outputDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    stop();
    this->inputDeviceIndex = inputDeviceIndex;
    this->outputDeviceIndex = outputDeviceIndex;
    this->firstInputIndex = firstIn;
    this->inputChannels = (lastIn - firstIn) + 1;
    this->firstOutputIndex = firstOut;
    this->outputChannels = (lastOut - firstOut) + 1;
    this->bufferSize = bufferSize;
    if(this->sampleRate != sampleRate){
        this->sampleRate = sampleRate;
        emit sampleRateChanged(this->sampleRate);
    }
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


