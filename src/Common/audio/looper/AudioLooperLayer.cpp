#include "AudioLooperLayer.h"
#include "../core/SamplesBuffer.h"
#include <cstring>

using namespace Audio;

LooperLayer::LooperLayer()
    : availableSamples(0),
      lastSamplesPerPeak(0),
      lastCacheComputationSample(0),
      locked(false),
      lastCycleLenght(0),
      gain(1.0),
      pan(0),
      leftGain(1),
      rightGain(1)
{
    //
}

LooperLayer::~LooperLayer()
{

}

void LooperLayer::zero()
{
    std::fill(leftChannel.begin(), leftChannel.end(), static_cast<float>(0));
    std::fill(rightChannel.begin(), rightChannel.end(), static_cast<float>(0));

    availableSamples = 0;
    lastSamplesPerPeak = 0;
    lastCacheComputationSample = 0;
    peaksCache.clear();
}

void LooperLayer::setPan(float pan)
{
    if (pan < -1)
        pan = -1;

    if (pan > 1)
        pan = 1;

    this->pan = pan;

    // update pain gain values
    const static double ROOT_2_OVER_2 = 1.414213562373095 *0.5;
    const static double PI_OVER_2 = 3.141592653589793238463 * 0.5;

    double angle = pan * PI_OVER_2 * 0.5;
    leftGain = (float)(ROOT_2_OVER_2 * (cos(angle) - sin(angle)));
    rightGain = (float)(ROOT_2_OVER_2 * (cos(angle) + sin(angle)));
}

void LooperLayer::setGain(float gain)
{
    this->gain = gain;
}

void LooperLayer::prepareForNewCycle(uint samplesInNewCycle, bool isOverdubbing)
{
    if (samplesInNewCycle > lastCycleLenght)
        resize(samplesInNewCycle);

    if (isOverdubbing)
        lastCacheComputationSample = 0; // reset max peak cache position to compute overdub peaks correctly

    lastCycleLenght = samplesInNewCycle;
}

void LooperLayer::overdub(const SamplesBuffer &samples, uint samplesToMix, uint startPosition)
{
    if (!samples.isMono()) {
        float *internalChannels[] = {&(leftChannel[startPosition]), &(rightChannel[startPosition])};
        float *samplesArray[] = {samples.getSamplesArray(0), samples.getSamplesArray(1)};
        for (uint s = 0; s < samplesToMix; ++s) {
            internalChannels[0][s] += samplesArray[0][s]; // left channel
            internalChannels[1][s] += samplesArray[1][s]; // right channel
        }
    }
    else {
        float *internalChannels[] = {&(leftChannel[startPosition]), &(rightChannel[startPosition])};
        float *samplesArray = samples.getSamplesArray(0);
        for (uint s = 0; s < samplesToMix; ++s) {
            internalChannels[0][s] += samplesArray[s];
        }
    }

    if (availableSamples < startPosition + samplesToMix)
        availableSamples = startPosition + samplesToMix;

    // build peaks cache when overdubbing
    if (lastSamplesPerPeak) {
        uint position = startPosition + samplesToMix;
        if (position - lastCacheComputationSample >= lastSamplesPerPeak) { // enough samples to cache a new max peak?
            const int peakIndex = (position/lastSamplesPerPeak) - 1;
            float lastPeak = computeMaxPeak(lastCacheComputationSample, lastSamplesPerPeak);
            if (peakIndex >= 0 && static_cast<uint>(peakIndex) < peaksCache.size())
                peaksCache[peakIndex] = lastPeak;
            else
                peaksCache.push_back(lastPeak);

            lastCacheComputationSample += lastSamplesPerPeak;
        }
    }
}

void LooperLayer::append(const SamplesBuffer &samples, uint samplesToAppend)
{
    samplesToAppend = qMin(static_cast<uint>(leftChannel.capacity() - availableSamples), samplesToAppend);
    const uint sizeInBytes = samplesToAppend * sizeof(float);

    const int secondChannelIndex = samples.isMono() ? 0 : 1;
    std::memcpy(&(leftChannel[0]) + availableSamples, samples.getSamplesArray(0), sizeInBytes);
    std::memcpy(&(rightChannel[0]) + availableSamples, samples.getSamplesArray(secondChannelIndex), sizeInBytes);

    availableSamples += samplesToAppend;

    Q_ASSERT(availableSamples <= leftChannel.size());

    // build peaks cache
    if (lastSamplesPerPeak) {
        if (availableSamples - lastCacheComputationSample >= lastSamplesPerPeak) { // enough samples to cache a new max peak?
            peaksCache.push_back(computeMaxPeak(lastCacheComputationSample, lastSamplesPerPeak));
            lastCacheComputationSample += lastSamplesPerPeak;
        }
    }
}

float LooperLayer::computeMaxPeak(uint from, uint samplesPerPeak) const
{
    float maxPeak = 0;
    uint limit = qMin(samplesPerPeak, availableSamples - from);
    for (uint i = 0; i < limit; ++i) {
        const uint offset = from + i;
        if (offset < leftChannel.size()) {
            float peak = qMax(qAbs(leftChannel[offset]), qAbs(rightChannel[offset]));
            if (peak > maxPeak) {
                maxPeak = peak;
            }
        }
    }

    return maxPeak;
}

std::vector<float> LooperLayer::getSamplesPeaks(uint samplesPerPeak)
{
    if (lastSamplesPerPeak == samplesPerPeak && !peaksCache.empty()) { // cache hit?
        return peaksCache;
    }

    // compute all peaks
    peaksCache.clear();
    if (samplesPerPeak) {
        for (uint i = 0; i < availableSamples; i += samplesPerPeak) {
            float maxPeak = computeMaxPeak(i, samplesPerPeak);
            peaksCache.push_back(maxPeak);
        }
        lastSamplesPerPeak = samplesPerPeak;
    }

    return peaksCache;
}

void LooperLayer::mixTo(SamplesBuffer &outBuffer, uint samplesToMix, uint intervalPosition)
{
    if (samplesToMix > 0) {
        float *internalChannels[] = {&(leftChannel[0]), &(rightChannel[0])};
        const uint secondChannelIndex = (outBuffer.isMono()) ? 0 : 1;
        float *bufferChannels[] = {outBuffer.getSamplesArray(0), outBuffer.getSamplesArray(secondChannelIndex)};
        uint channels = outBuffer.getChannels();

        const float finalLeftGain = gain * leftGain;
        const float finalRightGain = gain * rightGain;
        float gains[] = {finalLeftGain, finalRightGain};
        for (uint c = 0; c < channels; ++c) {
            for (uint s = 0; s < samplesToMix; ++s) {
                const uint offset = s + intervalPosition;
                bufferChannels[c][s] += internalChannels[c][offset] * gains[c];
            }
        }
    }
}

void LooperLayer::resize(quint32 samplesPerCycle)
{
    if (samplesPerCycle > leftChannel.capacity())
        leftChannel.resize(samplesPerCycle);

    if (samplesPerCycle > rightChannel.capacity())
        rightChannel.resize(samplesPerCycle);

    if (availableSamples && samplesPerCycle > availableSamples) { // need copy samples?
        uint initialAvailableSamples = availableSamples;
        uint totalSamplesToCopy = samplesPerCycle - initialAvailableSamples;
        while (totalSamplesToCopy > 0){
            const uint samplesToCopy = qMin(totalSamplesToCopy, initialAvailableSamples);
            const uint bytesToCopy = samplesToCopy * sizeof(float);
            std::memcpy(&(leftChannel[availableSamples]), &(leftChannel[0]), bytesToCopy);
            std::memcpy(&(rightChannel[availableSamples]), &(rightChannel[0]), bytesToCopy);
            availableSamples += samplesToCopy;
            totalSamplesToCopy -= samplesToCopy;
        }

        Q_ASSERT(availableSamples == samplesPerCycle);

        peaksCache.clear(); // invalidate peaks cache
    }
}
