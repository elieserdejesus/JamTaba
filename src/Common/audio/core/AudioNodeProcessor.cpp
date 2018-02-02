#include "AudioNodeProcessor.h"

#include "midi/MidiDriver.h"
#include "audio/core/SamplesBuffer.h"

using audio::AudioNodeProcessor;

AudioNodeProcessor::AudioNodeProcessor() :
    bypassed(false)
{

}

void AudioNodeProcessor::setBypass(bool state)
{
    if (state != bypassed)
        bypassed = state;
}
