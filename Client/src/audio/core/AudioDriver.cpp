#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <vector>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>

using namespace Audio;

//+++++++++++++++++++++

ChannelRange::ChannelRange(int firstChannel, int channelsCount)
    :firstChannel(firstChannel), channelsCount(channelsCount){
    if(firstChannel < 0 || channelsCount < 0){
        this->firstChannel = -1;
        this->channelsCount = 0;
    }
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
AudioDriver::AudioDriver(Controller::MainController *mainController)
    :
      globalInputRange(0, 0),
      globalOutputRange(0, 0),
      audioDeviceIndex(0),
    sampleRate(44100),
    bufferSize(128),

    inputBuffer(nullptr),
    outputBuffer(nullptr),

    //selectedInputs(0, 0),
    //selectedOutpus(0, 0),
    mainController(mainController)

{

}

void AudioDriver::recreateBuffers(){
//    if (inputBuffer){
//        delete inputBuffer;
//    }
    inputBuffer.reset( new SamplesBuffer(globalInputRange.getChannels()));


    outputBuffer.reset( new SamplesBuffer(globalOutputRange.getChannels()));
}

AudioDriver::~AudioDriver(){
    qDebug() << "AudioDriver destructor.";
    /*delete this->inputBuffer;
    delete this->outputBuffer*/;
}

void AudioDriver::setProperties(int sampleRate, int bufferSize){
    stop();

    this->bufferSize = bufferSize;

    if(this->sampleRate != sampleRate){
        this->sampleRate = sampleRate;
        emit sampleRateChanged(this->sampleRate);
    }
}

//void AudioDriver::setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
//{
//    setProperties(deviceIndex, firstIn, lastIn, firstOut, lastOut, sampleRate, bufferSize);
//}

void AudioDriver::setProperties(int audioDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize)
{
    stop();
    this->audioDeviceIndex = audioDeviceIndex;
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


