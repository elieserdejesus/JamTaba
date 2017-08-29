#include "MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"

using namespace Audio;

MetronomeTrackNode::MetronomeTrackNode(const SamplesBuffer &firstBeatSamples, const SamplesBuffer &secondaryBeatSamples) :
    firstBeatBuffer(firstBeatSamples),
    secondaryBeatBuffer(secondaryBeatSamples),
    samplesPerBeat(0),
    intervalPosition(0),
    beatPosition(0),
    currentBeat(0),
    beatsPerAccent(0)
{
    resetInterval();
}

MetronomeTrackNode::~MetronomeTrackNode()
{

}

void MetronomeTrackNode::setPrimaryBeatSamples(const SamplesBuffer &firstBeatSamples)
{
    firstBeatBuffer.set(firstBeatSamples);
}

void MetronomeTrackNode::setSecondaryBeatSamples(const SamplesBuffer &secondaryBeatSamples)
{
    secondaryBeatBuffer.set(secondaryBeatSamples);
}

void MetronomeTrackNode::setBeatsPerAccent(int beatsPerAccent)
{
    this->beatsPerAccent = beatsPerAccent;
}

void MetronomeTrackNode::setSamplesPerBeat(long samplesPerBeat)
{
    if (samplesPerBeat <= 0)
        qCritical() << "samples per beat <= 0";

    this->samplesPerBeat = samplesPerBeat;
    resetInterval();
}

void MetronomeTrackNode::resetInterval()
{
    beatPosition = intervalPosition = 0;
}

void MetronomeTrackNode::setIntervalPosition(long intervalPosition)
{
    if (samplesPerBeat <= 0)
        return;

    this->intervalPosition = intervalPosition;
    this->beatPosition = intervalPosition % samplesPerBeat;
    this->currentBeat = (intervalPosition / samplesPerBeat);
}

SamplesBuffer *MetronomeTrackNode::getSamplesBuffer(int beat)
{
    if (beat == 0 || (isPlayingAccents() && beat % beatsPerAccent == 0)) {
        return &firstBeatBuffer;
    }
    return &secondaryBeatBuffer;
}

void MetronomeTrackNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                          int SampleRate, std::vector<Midi::MidiMessage> &midiBuffer)
{
    if (samplesPerBeat <= 0)
        return;

    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();

    SamplesBuffer *samplesBuffer = getSamplesBuffer(currentBeat);
    uint samplesToCopy = qMin(static_cast<uint>(samplesBuffer->getFrameLenght() - beatPosition), out.getFrameLenght());
    int nextBeatSample = beatPosition + out.getFrameLenght();
    int internalOffset = 0;
    int samplesBufferOffset = beatPosition;
    if (nextBeatSample > samplesPerBeat) { // next beat starting in this audio buffer?
        samplesBuffer = getSamplesBuffer(currentBeat + 1);
        internalOffset = samplesPerBeat - beatPosition;
        samplesToCopy = std::min(nextBeatSample - samplesPerBeat,
                                 (long)samplesBuffer->getFrameLenght());
        samplesBufferOffset = 0;
    }

    if (samplesToCopy > 0)
        internalInputBuffer.set(*samplesBuffer, samplesBufferOffset, samplesToCopy,
                                internalOffset);
    AudioNode::processReplacing(in, out, SampleRate, midiBuffer);
}
