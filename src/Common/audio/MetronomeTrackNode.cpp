#include "MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"

using namespace Audio;

MetronomeTrackNode::MetronomeTrackNode(const SamplesBuffer &firstBeatSamples, const SamplesBuffer &offBeatSamples, const SamplesBuffer &accentBeatSamples) :
    firstBeatBuffer(firstBeatSamples),
    offBeatBuffer(offBeatSamples),
    accentBeatBuffer(accentBeatSamples),
    samplesPerBeat(0),
    intervalPosition(0),
    beatPosition(0),
    currentBeat(0)
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

void MetronomeTrackNode::setOffBeatSamples(const SamplesBuffer &offBeatSamples)
{
    offBeatBuffer.set(offBeatSamples);
}

void MetronomeTrackNode::setAccentBeatSamples(const SamplesBuffer &accentBeatSamples)
{
    accentBeatBuffer.set(accentBeatSamples);
}

int MetronomeTrackNode::getBeatsPerAccent() const
{
    if (this->accentBeats.length() > 0) {
        // TODO: determine whether accentBeats represents a repetitive sequence
        // TODO: if so, return frequency else return 0;
        return 0;
    } else {
        return 0;
    }
}

void MetronomeTrackNode::setBeatsPerAccent(int beatsPerAccent)
{
    QList<int> accents = QList<int>();
    for(int i = 1; i < 256; i++) {
        if (i % beatsPerAccent == 0) {
            accents.append(i);
        }
    }
    this->setAccentBeats(accents);
}

void MetronomeTrackNode::setAccentBeats(QList<int> accentBeats)
{
    this->accentBeats = accentBeats;
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
    if (beat == 0) {
        return &firstBeatBuffer;
    }
    if (isPlayingAccents()) {
        if (this->accentBeats.contains(beat)) {
            return &accentBeatBuffer;
        }
    }
    return &offBeatBuffer;
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
