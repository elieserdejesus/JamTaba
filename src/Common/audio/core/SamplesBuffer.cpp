#include "SamplesBuffer.h"
#include <QDebug>
#include <cmath>
#include <algorithm>
#include <cstring>

using namespace Audio;

const SamplesBuffer SamplesBuffer::ZERO_BUFFER(1, 0);

SamplesBuffer::SamplesBuffer(unsigned int channels) :
    SamplesBuffer(channels, 0)
{

}

SamplesBuffer::SamplesBuffer(unsigned int channels, unsigned int frameLenght) :
    channels(channels),
    frameLenght(frameLenght),
    rmsRunningSum(0.0f),
    summedSamples(0),
    rmsWindowSize(13230) // 300 ms in 44100 KHz
{
    for (unsigned int c = 0; c < channels; ++c)
        samples.push_back(std::vector<float>(frameLenght));

    squaredSums[0] = squaredSums[1] = 0.0f;
    lastRmsValues[0] = lastRmsValues[1] = 0.0f;
    summedSamples = 0;
}

SamplesBuffer::SamplesBuffer(const SamplesBuffer &other) :
      channels(other.channels),
      frameLenght(other.frameLenght),
      samples(other.samples),
      rmsRunningSum(other.rmsRunningSum),
      summedSamples(other.summedSamples),
      rmsWindowSize(other.rmsWindowSize)
{
    // qWarning() << "Samples Buffer copy constructor!";
    squaredSums[0] = other.squaredSums[0];
    squaredSums[1] = other.squaredSums[1];

    lastRmsValues[0] = other.lastRmsValues[0];
    lastRmsValues[1] = other.lastRmsValues[1];
}

SamplesBuffer &SamplesBuffer::operator=(const SamplesBuffer &other)
{
    this->channels = other.channels;
    this->frameLenght = other.frameLenght;
    this->rmsRunningSum = other.rmsRunningSum;
    this->rmsWindowSize = other.rmsWindowSize;
    this->summedSamples = other.summedSamples;

    squaredSums[0] = other.squaredSums[0];
    squaredSums[1] = other.squaredSums[1];

    lastRmsValues[0] = other.lastRmsValues[0];
    lastRmsValues[1] = other.lastRmsValues[1];

    this->samples = other.samples;

    return *this;
}

SamplesBuffer::~SamplesBuffer()
{
}

void SamplesBuffer::setRmsWindowSize(int samples)
{
    rmsWindowSize = samples;
}

void SamplesBuffer::invertStereo()
{
    if (channels != 2)
        return; // trying invert a non stereo buffer

    std::iter_swap(samples.begin(), samples.begin() + 1); // swap first and second channels
}

void SamplesBuffer::discardFirstSamples(unsigned int samplesToDiscard)
{
    int toDiscard = std::min(frameLenght, samplesToDiscard);
    int toCopy = frameLenght - toDiscard;
    uint newFrameLenght = frameLenght - toDiscard;
    for (uint c = 0; c < channels; ++c) {
        std::copy_n(samples[c].begin() + toDiscard, toCopy, samples[c].begin());
    }
    setFrameLenght(newFrameLenght);
}

void SamplesBuffer::append(const SamplesBuffer &other)
{
    int internalOffset = frameLenght;
    int newFrameLenght = frameLenght + other.frameLenght;
    setFrameLenght(newFrameLenght);
    set(other, 0, other.frameLenght, internalOffset);
}

float *SamplesBuffer::getSamplesArray(unsigned int channel) const
{
    Q_ASSERT(channel < samples.size());

    return const_cast<float *>(&(samples[channel][0]));
}

void SamplesBuffer::applyGain(float gainFactor, float boostFactor)
{
    const float scaleFactor = gainFactor * boostFactor;
    for (unsigned int c = 0; c < channels; ++c) {
        for (unsigned int i = 0; i < frameLenght; ++i)
            samples[c][i] *= scaleFactor;
    }
}

void SamplesBuffer::fadeOut(int fadeFrameLenght, float endGain)
{
    uint lenght = std::min(fadeFrameLenght, (int)frameLenght);
    float gainStep = (1 - endGain)/lenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = 1;
        for (unsigned int s = 0; s < lenght; ++s) {
            samples[c][s] *= gain;
            gain -= gainStep;
        }
    }
}

void SamplesBuffer::fadeIn(int fadeFrameLenght, float beginGain)
{
    uint lenght = std::min(fadeFrameLenght, (int)frameLenght);
    float gainStep = (1 - beginGain)/lenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = beginGain;
        for (unsigned int s = 0; s < lenght; ++s) {
            samples[c][s] *= gain;
            gain += gainStep;
        }
    }
}

void SamplesBuffer::fade(float beginGain, float endGain)
{
    float gainStep = (endGain - beginGain)/frameLenght;
    for (unsigned int c = 0; c < channels; ++c) {
        float gain = beginGain;
        for (unsigned int s = 0; s < frameLenght; ++s) {
            samples[c][s] *= gain;
            gain += gainStep;
        }
    }
}

void SamplesBuffer::applyGain(float gainFactor, float leftGain, float rightGain, float boostFactor)
{
    if (!isMono()) {
        float commonGain = gainFactor * boostFactor;
        float finalLeftGain = commonGain * leftGain;
        float finalRightGain = commonGain * rightGain;
        for (unsigned int i = 0; i < frameLenght; ++i) {
            samples[0][i] *= finalLeftGain;
            samples[1][i] *= finalRightGain;
        }
    }
    else {
        applyGain(gainFactor, boostFactor);
    }
}

void SamplesBuffer::zero()
{
    if (!frameLenght)
        return;

    const uint bytesToProcess = frameLenght * sizeof(float);
    for (unsigned int c = 0; c < channels; ++c) {
        Q_ASSERT(samples[c].size() >= frameLenght);
        memset(&(samples[c][0]), 0, bytesToProcess);
    }
}

AudioPeak SamplesBuffer::computePeak()
{
    float abs; // max peak absolute value
    float maxPeaks[2] = {0};// left and right peaks
    for (unsigned int c = 0; c < channels; ++c) {
        float maxPeak = 0;
        for (unsigned int i = 0; i < frameLenght; ++i) {

            // max peak
            abs = std::fabs(samples[c][i]);
            if (abs > maxPeak)
                maxPeak = abs;
            maxPeaks[c] = maxPeak;

            // rms running squared sum
            squaredSums[c] += samples[c][i] * samples[c][i]; // squaring every sample and summing
        }
        summedSamples += frameLenght;
    }
    if (isMono()) {
        maxPeaks[1] = maxPeaks[0];
        squaredSums[1] = squaredSums[0];
    }

    // time to compute new rms values?
    if (summedSamples >= rmsWindowSize) {
        lastRmsValues[0] = std::sqrt(squaredSums[0]/summedSamples);
        lastRmsValues[1] = std::sqrt(squaredSums[1]/summedSamples);

        squaredSums[0] = squaredSums[1] = 0.0f; // reinitialize the rms running sum
        summedSamples = 0;
    }

    return AudioPeak(maxPeaks[0], maxPeaks[1], lastRmsValues[0], lastRmsValues[1]);
}

int SamplesBuffer::computeRmsWindowSize(int sampleRate, int windowTimeInMs)
{
    return sampleRate * windowTimeInMs/1000.0f;
}

void SamplesBuffer::add(const SamplesBuffer &buffer, int internalWriteOffset)
{
    uint framesToProcess = std::min((uint)frameLenght, buffer.getFrameLenght());
    if (buffer.channels >= channels) {
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                Q_ASSERT(s + internalWriteOffset < samples[c].size());
                samples[c][s + internalWriteOffset] += buffer.samples[c][s];
            }
        }
    }
    else { // samples is stereo and buffer is mono
        for (unsigned int s = 0; s < framesToProcess; ++s) {
            Q_ASSERT(s + internalWriteOffset < samples[0].size());
            Q_ASSERT(s + internalWriteOffset < samples[1].size());
            samples[0][s + internalWriteOffset] += buffer.samples[0][s];
            samples[1][s + internalWriteOffset] += buffer.samples[0][s];
        }
    }
}

void SamplesBuffer::add(uint channel, float *samples, uint samplesToAdd)
{
    Q_ASSERT(channel < channels && channels == this->samples.size());
    Q_ASSERT(samplesToAdd <= frameLenght && samplesToAdd <= this->samples[channel].size());

    void *dest = &(this->samples[channel][0]);
    const uint bytesToCopy = std::min(static_cast<uint>(frameLenght), samplesToAdd) * sizeof(float);
    memcpy(dest, samples, bytesToCopy);
}

void SamplesBuffer::add(uint channel, uint sampleIndex, float sampleValue)
{
    Q_ASSERT(channel < channels && channels == samples.size());
    Q_ASSERT(sampleIndex < samples[channel].size());

    samples[channel][sampleIndex] += sampleValue;
}

void SamplesBuffer::set(uint channel, uint sampleIndex, float sampleValue)
{
    Q_ASSERT(channel < channels && channels <= samples.size());
    Q_ASSERT(sampleIndex < samples[channel].size());

    samples[channel][sampleIndex] = sampleValue;
}

void SamplesBuffer::setToMono()
{
    if (this->channels != 1)
        this->channels = 1;
}

void SamplesBuffer::setToStereo()
{
    if (this->samples.size() < 2) {
        size_t channelsToAdd = 2 - this->samples.size();
        for (uint c = 0; c < channelsToAdd; ++c)
            samples.push_back(std::vector<float>(frameLenght));
    }

    for (unsigned int c = 0; c < samples.size(); ++c)
        samples[c].resize(frameLenght);

    this->channels = 2;
}

void SamplesBuffer::set(const SamplesBuffer &buffer)
{
    set(buffer, 0, std::min(buffer.frameLenght, frameLenght), 0);
}

float SamplesBuffer::get(uint channel, uint sampleIndex) const
{
    Q_ASSERT(channel < channels);
    Q_ASSERT(sampleIndex < samples[channel].capacity());

    return samples[channel][sampleIndex];
}

void SamplesBuffer::setFrameLenght(unsigned int newFrameLenght)
{
    if (newFrameLenght == frameLenght)
        return;

    if (newFrameLenght > frameLenght) {
        for (unsigned int c = 0; c < channels; ++c)
            samples[c].resize(newFrameLenght);
    }
    this->frameLenght = newFrameLenght;
}

void SamplesBuffer::set(const SamplesBuffer &buffer, int bufferChannelOffset, int channelsToCopy)
{
    if (buffer.channels == 0 || channels == 0)
        return;

    int framesToCopy = std::min(buffer.getFrameLenght(), frameLenght);
    int channelsToProcess = std::min(channelsToCopy, std::min(buffer.getChannels(), static_cast<int>(channels)));

    Q_ASSERT(channelsToProcess + bufferChannelOffset <= buffer.getChannels());

    int bytesToCopy = framesToCopy * sizeof(float);
    for (int c = 0; c < channelsToProcess; ++c) {
        memcpy((void *)getSamplesArray(c), buffer.getSamplesArray(c + bufferChannelOffset), bytesToCopy);
    }
}

void SamplesBuffer::set(const SamplesBuffer &buffer, uint bufferOffset, uint samplesToCopy, uint internalOffset)
{
    if (buffer.channels == 0 || channels == 0)
        return;

    unsigned int framesToProcess = std::min(samplesToCopy, buffer.getFrameLenght() - bufferOffset);
    if (framesToProcess + bufferOffset > buffer.frameLenght) // fixing bug in some built-in metronome sounds
        return ;

    if ((uint)(internalOffset + framesToProcess) > this->getFrameLenght())
        framesToProcess = (internalOffset + framesToProcess) - this->getFrameLenght();

    const uint bytesToProcess = framesToProcess * sizeof(float);
    if (!bytesToProcess)
        return;

    if (channels == buffer.channels) {// channels number are equal
        for (unsigned int c = 0; c < channels; ++c) {
            std::memcpy(&(samples[c][internalOffset]), &(buffer.samples[c][bufferOffset]), bytesToProcess);
        }
    }
    else { // different number of channels
        if (!isMono()) { // copy every &buffer samples to LR in this buffer
            if (!buffer.isMono()) {
                int channelsToCopy = qMin(channels, buffer.channels);
                for (int c = 0; c < channelsToCopy; ++c) {
                    Q_ASSERT(internalOffset < samples[c].size());
                    Q_ASSERT(bufferOffset < buffer.samples[c].size());
                    Q_ASSERT(bufferOffset + framesToProcess < buffer.samples[c].size());
                    Q_ASSERT(internalOffset + framesToProcess < samples[c].size());
                    std::memcpy(&(samples[c][internalOffset]), &(buffer.samples[c][bufferOffset]), bytesToProcess);
                }
            } else {
                std::memcpy(&(samples[0][internalOffset]), &(buffer.samples[0][bufferOffset]), bytesToProcess);
                std::memcpy(&(samples[1][internalOffset]), &(buffer.samples[0][bufferOffset]), bytesToProcess);
            }
        } else { // this buffer is mono, but the buffer in parameter is not! Mix down the stereo samples in one mono sample value.
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                const int index = s + bufferOffset;
                float v = (buffer.samples[0][index] + buffer.samples[1][index])/2.0f;
                samples[0][s + internalOffset] = v;
            }
        }
    }
}
