#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <vector>
#include <QDebug>
#include <cmath>
#include <QMutexLocker>
#include "log/Logging.h"

using audio::ChannelRange;
using audio::AudioDriver;

ChannelRange::ChannelRange(int firstChannel, int channelsCount) :
    firstChannel(firstChannel),
    channelsCount(channelsCount)
{
    if (firstChannel < 0 || channelsCount < 0) {
        this->firstChannel = -1;
        this->channelsCount = 0;
    }
}

ChannelRange::ChannelRange() :
    firstChannel(-1),
    channelsCount(0)
{

}

void ChannelRange::setToStereo()
{
    this->channelsCount = 2;
}

void ChannelRange::setToMono()
{
    this->channelsCount = 1;
}

// +++++++++++++++++++++++++++++++++++++++++++
AudioDriver::AudioDriver(controller::MainController *mainController) :
    globalInputRange(0, 0),
    globalOutputRange(0, 0),
    audioInputDeviceIndex(-1),
    audioOutputDeviceIndex(-1),
    sampleRate(44100),
    bufferSize(128),
    inputBuffer(SamplesBuffer(2)),
    outputBuffer(SamplesBuffer(2)),
    mainController(mainController)
{

}

void AudioDriver::recreateBuffers()
{
    inputBuffer = SamplesBuffer(globalInputRange.getChannels());
    outputBuffer = SamplesBuffer(globalOutputRange.getChannels());
}

AudioDriver::~AudioDriver()
{
    qCDebug(jtAudio) << "AudioDriver destructor.";
}

void AudioDriver::setProperties(int firstIn, int lastIn, int firstOut, int lastOut)
{
    stop();
    this->globalInputRange = ChannelRange(firstIn, (lastIn - firstIn) + 1);
    this->globalOutputRange = ChannelRange(firstOut, (lastOut - firstOut) + 1);
}

void AudioDriver::setSampleRate(int newSampleRate)
{
    if (sampleRate != newSampleRate) {
        sampleRate = newSampleRate;
        emit sampleRateChanged(newSampleRate);
    }
}

void AudioDriver::setBufferSize(int newBufferSize)
{
    bufferSize = newBufferSize;
}
