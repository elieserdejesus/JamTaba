#include "AudioUnitHost.h"

#include <CoreAudio/CoreAudio.h>
#include <QDebug>

using namespace AU;

QScopedPointer<AudioUnitHost> AudioUnitHost::instance;

AudioUnitHost * AudioUnitHost::getInstance()
{
    if (instance.isNull())
        instance.reset(new AudioUnitHost());

    return instance.data();
}

AudioUnitHost::AudioUnitHost()
    : sampleRate(44100),
      bufferSize(256),
      tempo(120),
      timeSignatureNumerator(4),
      timeSignatureDenominator(4),
      playing(false),
      beat(0)
{

}

std::vector<Midi::MidiMessage> AudioUnitHost::pullReceivedMidiMessages()
{
    return std::vector<Midi::MidiMessage>();
}

void AudioUnitHost::setSampleRate(int sampleRate)
{
    if (sampleRate != this->sampleRate)
        this->sampleRate = sampleRate;
}

void AudioUnitHost::setBlockSize(int blockSize)
{
    if (blockSize != this->bufferSize)
        this->bufferSize = blockSize;
}

void AudioUnitHost::setTempo(int bpm)
{
    this->tempo = bpm;
}

void AudioUnitHost::setPlayingFlag(bool playing)
{
    this->playing = playing;
}


void AudioUnitHost::setPositionInSamples(int intervalPosition)
{
    this->position = intervalPosition;
}
