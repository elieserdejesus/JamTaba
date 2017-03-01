#include "AudioLooperLayer.h"
#include "../core/SamplesBuffer.h"
#include <cstring>

using namespace Audio;

LooperLayer::LooperLayer()
    : availableSamples(0),
      lastSamplesPerPeak(0),
      lastCacheComputationSample(0),
      locked(false)
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

void LooperLayer::prepareForNewCycle(uint samplesInNewCycle, bool isOverdubbing)
{
    if (samplesInNewCycle > leftChannel.capacity())
        resize(samplesInNewCycle);

    if (isOverdubbing)
        lastCacheComputationSample = 0; // reset max peak cache position to compute overdub peaks correctly
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
    const uint sizeInBytes = samplesToAppend * sizeof(float);

    std::memcpy(&(leftChannel[0]) + availableSamples, samples.getSamplesArray(0), sizeInBytes);
    const int secondChannelIndex = samples.isMono() ? 0 : 1;
    std::memcpy(&(rightChannel[0]) + availableSamples, samples.getSamplesArray(secondChannelIndex), sizeInBytes);

    availableSamples += samplesToAppend;

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
        //if (!replacing) {
            for (uint c = 0; c < channels; ++c) {
                for (uint s = 0; s < samplesToMix; ++s) {
                    const uint offset = s + intervalPosition;
                    bufferChannels[c][s] += internalChannels[c][offset];
                }
            }
        //}
//        else { // replacing
//            uint bytes = samplesToMix * sizeof(float);
//            for (uint c = 0; c < channels; ++c) {
//                std::memcpy(bufferChannels[c], internalChannels[c] + intervalPosition, bytes);
//            }
//        }
    }
}

void LooperLayer::resize(quint32 samples)
{
    if (samples > leftChannel.capacity())
        leftChannel.resize(samples);

    if (samples > rightChannel.capacity())
        rightChannel.resize(samples);

    if (availableSamples && samples > availableSamples) { // need copy samples?
        uint initialAvailableSamples = availableSamples;
        uint totalSamplesToCopy = samples - initialAvailableSamples;
        while (totalSamplesToCopy > 0){
            const uint samplesToCopy = qMin(totalSamplesToCopy, initialAvailableSamples);
            const uint bytesToCopy = samplesToCopy * sizeof(float);
            std::memcpy(&(leftChannel[availableSamples]), &(leftChannel[0]), bytesToCopy);
            std::memcpy(&(rightChannel[availableSamples]), &(rightChannel[0]), bytesToCopy);
            availableSamples += samplesToCopy;
            totalSamplesToCopy -= samplesToCopy;
        }

        Q_ASSERT(availableSamples == samples);

        peaksCache.clear(); // invalidate peaks cache
    }
}
