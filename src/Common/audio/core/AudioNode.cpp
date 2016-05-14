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

using namespace Audio;

const double AudioNode::ROOT_2_OVER_2 = 1.414213562373095 *0.5;
const double AudioNode::PI_OVER_2 = 3.141592653589793238463 * 0.5;

// +++++++++++++++

void AudioNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                 const Midi::MidiMessageBuffer &midiBuffer)
{
    Q_UNUSED(in);

    if (!isActivated())
        return;

    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght());

    {
        QMutexLocker locker(&mutex);
        foreach (AudioNode *node, connections)  // ask connected nodes to generate audio
            node->processReplacing(internalInputBuffer, internalOutputBuffer, sampleRate,
                                   midiBuffer);
    }

    internalOutputBuffer.set(internalInputBuffer);// if we have no plugins insert the input samples are just copied  to output buffer.


    static SamplesBuffer tempInputBuffer(2);

    QList<Midi::MidiMessage> midiMessages = midiBuffer.toList();

    // process inserted plugins
    for (int i=0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        AudioNodeProcessor *processor = processors[i];
        if (processor && !processor->isBypassed()) {
            tempInputBuffer.setFrameLenght(internalOutputBuffer.getFrameLenght());
            tempInputBuffer.set(internalOutputBuffer); //the output from previous plugin is used as input to the next plugin in the chain

            processor->process(tempInputBuffer, internalOutputBuffer, midiMessages);

            // some plugins are blocking the midi messages. If a VSTi can't generate messages the previous messages list will be sended for the next plugin in the chain. The messages list is cleared only when the plugin can generate midi messages.
            if (processor->isVirtualInstrument() && processor->canGenerateMidiMessages())
                midiMessages.clear(); // only the fresh messages will be passed by the next plugin in the chain

            midiMessages.append(pullMidiMessagesGeneratedByPlugins());
        }
    }


    internalOutputBuffer.applyGain(gain, leftGain, rightGain, boost);

    lastPeak.update(internalOutputBuffer.computePeak());

    out.add(internalOutputBuffer);
}

AudioNode::AudioNode() :
    internalInputBuffer(2),
    internalOutputBuffer(2),
    lastPeak(0, 0),
    muted(false),
    soloed(false),
    activated(true),
    gain(1),
    boost(1),
    pan(0),
    leftGain(1.0),
    rightGain(1.0),
    resamplingCorrection(0)
{
    for(int i=0; i < MAX_PROCESSORS_PER_TRACK; ++i)
        processors[i] = nullptr;
}

QList<Midi::MidiMessage> AudioNode::pullMidiMessagesGeneratedByPlugins() const
{
    return QList<Midi::MidiMessage>(); // returning empty list by default, is overrided in LocalInputNode
}

int AudioNode::getInputResamplingLength(int sourceSampleRate, int targetSampleRate,
                                        int outFrameLenght)
{
    double doubleValue = (double)sourceSampleRate*(double)outFrameLenght/(double)targetSampleRate;
    int intValue = (int)doubleValue;
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

Audio::AudioPeak AudioNode::getLastPeak() const
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
