#include "MidiSyncTrackNode.h"
#include "MainController.h"
#include "MetronomeUtils.h"
#include "audio/core/AudioDriver.h"
#include <algorithm>

using audio::MidiSyncTrackNode;
using audio::SamplesBuffer;

MidiSyncTrackNode::MidiSyncTrackNode(MainController *controller) :
    pulsesPerInterval(0),
    samplesPerPulse(0),
    intervalPosition(0),
    currentPulse(0),
    lastPlayedPulse(-1),
    running(false),
    hasSentStart(false),
    mainController(controller)
{
    resetInterval();
}

MidiSyncTrackNode::~MidiSyncTrackNode()
{
}

void MidiSyncTrackNode::setPulseTiming(long pulsesPerInterval, double samplesPerPulse)
{
    if (pulsesPerInterval <= 0 || samplesPerPulse <= 0)
        qCritical() << "invalid sync timing params";

    this->pulsesPerInterval = pulsesPerInterval;
    this->samplesPerPulse = samplesPerPulse;
    resetInterval();
}

void MidiSyncTrackNode::resetInterval()
{
    intervalPosition = 0;
    lastPlayedPulse = -1;
}

void MidiSyncTrackNode::setIntervalPosition(long intervalPosition)
{
    if (samplesPerPulse <= 0)
        return;

    this->intervalPosition = intervalPosition;
    this->currentPulse = ((double)intervalPosition / samplesPerPulse);
}

void MidiSyncTrackNode::start()
{
    running = true;
}

void MidiSyncTrackNode::stop()
{
    running = false;
    hasSentStart = false;
    mainController->stopMidiClock();
}

void MidiSyncTrackNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                          int SampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    if (pulsesPerInterval <= 0 || samplesPerPulse <= 0)
        return;

    if (currentPulse == 0 && currentPulse != lastPlayedPulse) {
        if (running)  {
            if (!hasSentStart) {
                mainController->startMidiClock();
                hasSentStart = true;
            }
            // Attempt to reset each interval - need to make optional
//            else {
//                mainController->stopMidiClock();
//                mainController->continueMidiClock();
//            }
        }
//        qDebug() << "Pulses played in interval: " << lastPlayedPulse;
        lastPlayedPulse = -1;
    }

    while (currentPulse < pulsesPerInterval && currentPulse - lastPlayedPulse >= 1) {
        mainController->sendMidiClockPulse();
        lastPlayedPulse++;
    }
    AudioNode::processReplacing(in, out, SampleRate, midiBuffer);
}
