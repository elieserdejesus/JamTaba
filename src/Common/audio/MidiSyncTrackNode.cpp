#include "MidiSyncTrackNode.h"
#include "MainController.h"
#include "MetronomeUtils.h"
#include "audio/core/AudioDriver.h"
#include <algorithm>

using audio::MidiSyncTrackNode;
using audio::SamplesBuffer;

MidiSyncTrackNode::MidiSyncTrackNode(MainController *controller) :
    samplesPerPulse(0),
    intervalPosition(0),
    pulsePosition(0),
    currentPulse(0),
    lastPlayedPulse(-1),
    mainController(controller)
{
    resetInterval();
}

MidiSyncTrackNode::~MidiSyncTrackNode()
{
    mainController->stopMidiClock();
}

void MidiSyncTrackNode::setSamplesPerPulse(long samplesPerPulse)
{
    if (samplesPerPulse <= 0)
        qCritical() << "samples per pulse <= 0";

    this->samplesPerPulse = samplesPerPulse;
    resetInterval();
}

void MidiSyncTrackNode::resetInterval()
{
    pulsePosition = intervalPosition = 0;
    lastPlayedPulse = -1;
}

void MidiSyncTrackNode::setIntervalPosition(long intervalPosition)
{
    if (samplesPerPulse <= 0)
        return;

    const long latencyOffset = 512;

    intervalPosition += latencyOffset;

    // new interval, reset the things
    if (intervalPosition < this->intervalPosition) {
        this->lastPlayedPulse = -1;
    }

    this->intervalPosition = intervalPosition;
    this->pulsePosition = intervalPosition % samplesPerPulse;
    this->currentPulse = intervalPosition / samplesPerPulse;
}

void MidiSyncTrackNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                          int SampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    if (samplesPerPulse <= 0)
        return;

    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();

    int nextPulseSample = pulsePosition + out.getFrameLenght();
    if (nextPulseSample > samplesPerPulse && currentPulse > lastPlayedPulse) { // next pulse starting in this audio buffer?
        if (currentPulse == 0){
            mainController->stopMidiClock();
            mainController->startMidiClock();
        }
        while (currentPulse - lastPlayedPulse >= 1) {
            mainController->sendMidiClockPulse();
            lastPlayedPulse++;
        }
    }

    AudioNode::processReplacing(in, out, SampleRate, midiBuffer);
}
