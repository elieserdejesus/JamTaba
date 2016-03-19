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
                                 const Midi::MidiBuffer &midiBuffer)
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

    if (!processors.isEmpty()) {
        static SamplesBuffer tempInputBuffer(2);
        // process inserted plugins
        foreach (AudioNodeProcessor *processor, processors) {
            if (!processor->isBypassed()) {
                tempInputBuffer.setFrameLenght(internalOutputBuffer.getFrameLenght());
                tempInputBuffer.set(internalOutputBuffer);
                processor->process(tempInputBuffer, internalOutputBuffer, midiBuffer);
            }
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
    foreach (AudioNodeProcessor *processor, processors)
        delete processor;
    processors.clear();
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

void AudioNode::addProcessor(AudioNodeProcessor *newProcessor)
{
    assert(newProcessor);
    processors.append(newProcessor);
}

void AudioNode::removeProcessor(AudioNodeProcessor *processor)
{
    assert(processor);
    processor->suspend();
    processors.removeOne(processor);

    delete processor;
}

void AudioNode::suspendProcessors()
{
    foreach (AudioNodeProcessor *processor, processors)
        processor->suspend();
}

void AudioNode::updateProcessorsGui()
{
    QMutexLocker locker(&mutex);
    foreach (AudioNodeProcessor *processor, processors)
        processor->updateGui();
}

void AudioNode::resumeProcessors()
{
    foreach (AudioNodeProcessor *processor, processors)
        processor->resume();
}
