#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <vector>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>

using namespace Audio;

ChannelRange::ChannelRange(int firstChannel, int channelsCount)
    :firstChannel(firstChannel), channelsCount(channelsCount){
}

ChannelRange::ChannelRange()
    :firstChannel(-1), channelsCount(0)
{

}

void ChannelRange::setToStereo(){
    this->channelsCount = 2;
}

void ChannelRange::setToMono(){
    this->channelsCount = 1;
}

//+++++++++++++++++++++++++++++++++++++++++++
AudioDriver::AudioDriver(AudioDriverListener *audioDriverListener)
    :
    inputDeviceIndex(0),//index of selected device index. In ASIO the inputDeviceIndex and outputDeviceIndex are equal.
    outputDeviceIndex(0),
    sampleRate(44100),
    bufferSize(128),
    inputBuffer(nullptr),
    outputBuffer(nullptr),
    globalInputRange(0, 0),
    globalOutputRange(0, 0),
    //selectedInputs(0, 0),
    //selectedOutpus(0, 0),
    audioDriverListener(audioDriverListener)

{

}

void AudioDriver::recreateBuffers(){
    if (inputBuffer){
        delete inputBuffer;
    }
    inputBuffer = new SamplesBuffer(globalInputRange.getChannels());

    if(outputBuffer)
    if (outputBuffer != NULL){
        delete outputBuffer;
    }
    outputBuffer = new SamplesBuffer(globalOutputRange.getChannels());
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
    this->globalInputRange = ChannelRange(firstIn, (lastIn - firstIn) + 1);
    this->globalOutputRange = ChannelRange(firstOut, (lastOut - firstOut) + 1);
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


