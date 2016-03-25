#include "MetronomeTrackNode.h"
#include "audio/core/AudioDriver.h"
#include "audio/core/SamplesBuffer.h"
#include "audio/SamplesBufferResampler.h"
#include "Resampler.h"

using namespace Audio;

SamplesBuffer *createResampledBuffer(const SamplesBuffer &buffer, int originalSampleRate,
                                     int finalSampleRate)
{
    int finalSize = (double)finalSampleRate/originalSampleRate * buffer.getFrameLenght();
    int channels = buffer.getChannels();
    SamplesBuffer *newBuffer = new SamplesBuffer(channels, finalSize);
    for (int c = 0; c < channels; ++c) {
        ResamplerTest resampler;
        resampler.process(buffer.getSamplesArray(c),
                          buffer.getFrameLenght(), newBuffer->getSamplesArray(c), finalSize);
    }
    return newBuffer;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MetronomeTrackNode::~MetronomeTrackNode()
{

}
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setPrimaryBeatSamples(const SamplesBuffer &firstBeatSamples)
{
    firstBeatBuffer.set(firstBeatSamples);
}

void MetronomeTrackNode::setSecondaryBeatSamples(const SamplesBuffer &secondaryBeatSamples)
{
    secondaryBeatBuffer.set(secondaryBeatSamples);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//void MetronomeTrackNode::createDefaultMetronomeSounds()
//{
//    secondaryBeatBuffer = readWavFile(metronomeWaveFile, this->waveFileSampleRate);// the last param value will be changed by readWavFile method
//    if (waveFileSampleRate != (uint)localSampleRate) {
//        SamplesBuffer *temp = secondaryBeatBuffer;
//        secondaryBeatBuffer = createResampledBuffer(*secondaryBeatBuffer, waveFileSampleRate,
//                                                 localSampleRate);
//        delete temp;
//    }

//    firstBeatBuffer = createResampledBuffer(*secondaryBeatBuffer, localSampleRate,
//                                                    localSampleRate * 0.5);
//    firstMeasureBeatBuffer = createResampledBuffer(*secondaryBeatBuffer, localSampleRate,
//                                                   localSampleRate * 0.75);
//}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setBeatsPerAccent(int beatsPerAccent)
{
    this->beatsPerAccent = beatsPerAccent;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setSamplesPerBeat(long samplesPerBeat)
{
    if (samplesPerBeat <= 0)
        qCritical() << "samples per beat <= 0";
    this->samplesPerBeat = samplesPerBeat;
    resetInterval();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::resetInterval()
{
    beatPosition = intervalPosition = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::setIntervalPosition(long intervalPosition)
{
    if (samplesPerBeat <= 0)
        return;
    this->intervalPosition = intervalPosition;
    this->beatPosition = intervalPosition % samplesPerBeat;
    this->currentBeat = (intervalPosition / samplesPerBeat);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SamplesBuffer *MetronomeTrackNode::getSamplesBuffer(int beat)
{
    if (beat == 0 || (isPlayingAccents() && beat % beatsPerAccent == 0)){
        return &firstBeatBuffer;
    }
    return &secondaryBeatBuffer;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void MetronomeTrackNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                          int SampleRate, const Midi::MidiMessageBuffer &midiBuffer)
{
    if (samplesPerBeat <= 0)
        return;
    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();

    SamplesBuffer *samplesBuffer = getSamplesBuffer(currentBeat);
    int samplesToCopy = std::min(
        (int)(samplesBuffer->getFrameLenght() - beatPosition), out.getFrameLenght());
    int nextBeatSample = beatPosition + out.getFrameLenght();
    int internalOffset = 0;
    int clickSoundBufferOffset = beatPosition;
    if (nextBeatSample > samplesPerBeat) {// next beat starting in this audio buffer?
        samplesBuffer = getSamplesBuffer(currentBeat + 1);
        internalOffset = samplesPerBeat - beatPosition;
        samplesToCopy = std::min(nextBeatSample - samplesPerBeat,
                                 (long)samplesBuffer->getFrameLenght());
        clickSoundBufferOffset = 0;
    }
    if (samplesToCopy > 0)
        internalInputBuffer.set(*samplesBuffer, clickSoundBufferOffset, samplesToCopy,
                                internalOffset);
    AudioNode::processReplacing(in, out, SampleRate, midiBuffer);
}
