#include "AudioNode.h"
#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include "AudioNodeProcessor.h"
#include "AudioPeak.h"
#include <cmath>
#include <cassert>
#include <QDebug>
#include "midi/MidiDriver.h"
#include <QMutexLocker>

#include "audio/Resampler.h"

using audio::AudioNode;
using audio::SamplesBuffer;
using audio::AudioPeak;
using audio::AudioNodeProcessor;

const double AudioNode::ROOT_2_OVER_2 = 1.414213562373095 * 0.5;
const double AudioNode::PI_OVER_2 = 3.141592653589793238463 * 0.5;

void AudioNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<midi::MidiMessage> &midiBuffer)
{
    Q_UNUSED(in);

    if (!isActivated())
        return;

    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght());

    {
        QMutexLocker locker(&mutex);
        for (auto node : connections) { // ask connected nodes to generate audio
            node->processReplacing(internalInputBuffer, internalOutputBuffer, sampleRate, midiBuffer);
        }
    }

    internalOutputBuffer.set(internalInputBuffer); // if we have no plugins inserted the input samples are just copied  to output buffer.

    static SamplesBuffer tempInputBuffer(2);

    // process inserted plugins
    for (int i=0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        auto processor = processors[i];
        if (processor && !processor->isBypassed()) {
            tempInputBuffer.setFrameLenght(internalOutputBuffer.getFrameLenght());
            tempInputBuffer.set(internalOutputBuffer); // the output from previous plugin is used as input to the next plugin in the chain

            processor->process(tempInputBuffer, internalOutputBuffer, midiBuffer);

            // some plugins are blocking the midi messages. If a VSTi can't generate messages the previous messages list will be sended for the next plugin in the chain. The messages list is cleared only when the plugin can generate midi messages.
            if (processor->isVirtualInstrument() && processor->canGenerateMidiMessages())
                midiBuffer.clear(); // only the fresh messages will be passed by the next plugin in the chain


            auto pulledMessages = pullMidiMessagesGeneratedByPlugins();
            midiBuffer.insert(midiBuffer.end(), pulledMessages.begin(), pulledMessages.end());
        }
    }

    preFaderProcess(internalOutputBuffer); //call overrided preFaderProcess in subclasses to allow some preFader process.

    internalOutputBuffer.applyGain(gain, leftGain, rightGain, boost);

    lastPeak.update(internalOutputBuffer.computePeak());

    postFaderProcess(internalOutputBuffer);

    out.add(internalOutputBuffer);
}

void AudioNode::setRmsWindowSize(int samples)
{
    internalOutputBuffer.setRmsWindowSize(samples);
}

AudioNode::AudioNode() :
    internalInputBuffer(2),
    internalOutputBuffer(2),
    lastPeak(),
    pan(0),
    leftGain(1.0),
    rightGain(1.0),
    muted(false),
    soloed(false),
    activated(true),
    gain(1),
    boost(1),
    resamplingCorrection(0)
{

    for (int i=0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        processors[i] = nullptr;
    }
}

std::vector<midi::MidiMessage> AudioNode::pullMidiMessagesGeneratedByPlugins() const
{
    return std::vector<midi::MidiMessage>(); // returning empty vector by default, is overrided in LocalInputNode
}

int AudioNode::getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght)
{
    double doubleValue = static_cast<double>(sourceSampleRate) * static_cast<double>(outFrameLenght) / static_cast<double>(targetSampleRate);
    int intValue = static_cast<int>(doubleValue);
    resamplingCorrection += doubleValue - intValue;
    if (qAbs(resamplingCorrection) > 1) {
        intValue += resamplingCorrection;
        if (resamplingCorrection > 0)
            resamplingCorrection--;
        else
            resamplingCorrection++;
    }

    return intValue;
}

AudioPeak AudioNode::getLastPeak() const
{
    return this->lastPeak;
}

void AudioNode::resetLastPeak()
{
    lastPeak.zero();
}

void AudioNode::setPan(float pan)
{
    if (pan < -1)
        pan = -1;

    if (pan > 1)
        pan = 1;

    this->pan = pan;

    updateGains();

    emit panChanged(this->pan);
}

void AudioNode::setGain(float gainValue)
{
    this->gain = gainValue;

    emit gainChanged(this->gain);
}

void AudioNode::setBoost(float boostValue)
{
    this->boost = boostValue;

    emit boostChanged(this->boost);
}

void AudioNode::setMute(bool muteStatus)
{
    if (this->muted != muteStatus) {
        this->muted = muteStatus;
        emit muteChanged(muteStatus);
    }
}

void AudioNode::setSolo(bool soloed)
{
    if (this->soloed != soloed) {
        this->soloed = soloed;
        emit soloChanged(this->soloed);
    }
}

void AudioNode::reset()
{
    setGain(1.0);
    setPan(0);
    setBoost(1.0);
    setMute(false);
    setSolo(false);
}

void AudioNode::updateGains()
{
    double angle = pan * PI_OVER_2 * 0.5;
    leftGain = (float)(ROOT_2_OVER_2 * (cos(angle) - sin(angle)));
    rightGain = (float)(ROOT_2_OVER_2 * (cos(angle) + sin(angle)));
}

AudioNode::~AudioNode()
{
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i]){
            delete processors[i];
            processors[i] = nullptr;
        }
    }
}

bool AudioNode::connect(AudioNode &other)
{
    QMutexLocker(&(other.mutex));

    other.connections.insert(this);

    return true;
}

bool AudioNode::disconnect(AudioNode &otherNode)
{
    QMutexLocker(&(otherNode.mutex));
    otherNode.connections.remove(this);
    return true;
}

void AudioNode::addProcessor(AudioNodeProcessor *newProcessor, quint32 slotIndex)
{
    assert(newProcessor);
    assert(slotIndex < MAX_PROCESSORS_PER_TRACK);
    processors[slotIndex] = newProcessor;
}

void AudioNode::removeProcessor(AudioNodeProcessor *processor)
{
    assert(processor);
    processor->suspend();
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i] == processor){
            processors[i] = nullptr;
            break;
        }
    }
    delete processor;
}

void AudioNode::suspendProcessors()
{
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i])
            processors[i]->suspend();
    }
}

void AudioNode::updateProcessorsGui()
{
    QMutexLocker locker(&mutex);
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i])
            processors[i]->updateGui();
    }
}

void AudioNode::resumeProcessors()
{
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i])
            processors[i]->resume();
    }
}
