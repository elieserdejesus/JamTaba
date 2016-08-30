#include "SamplesBuffer.h"
#include <QDebug>
#include <cmath>
#include <algorithm>

using namespace Audio;
// +++++++++++++++++=

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

const SamplesBuffer SamplesBuffer::ZERO_BUFFER(1, 0);

SamplesBuffer::SamplesBuffer(unsigned int channels) :
    channels(channels),
    frameLenght(0),
    rmsRunningSum(0.0f),
    rmsWindowSize(13230) //300 ms in 44100 KHz
{
    if (channels == 0)
        qCritical() << "AudioSamplesBuffer::channels == 0";
    for (unsigned int c = 0; c < channels; ++c)
        samples.push_back(std::vector<float>());

    squaredSums[0] = squaredSums[1] = 0.0f;
    lastRmsValues[0] = lastRmsValues[1] = 0.0f;
    summedSamples = 0;
}

SamplesBuffer::SamplesBuffer(unsigned int channels, unsigned int frameLenght) :
    channels(channels),
    frameLenght(frameLenght),
    rmsRunningSum(0.0f),
    rmsWindowSize(13230) //300 ms in 44100 KHz
{
    for (unsigned int c = 0; c < channels; ++c)
        samples.push_back(std::vector<float>(frameLenght));
}

SamplesBuffer::SamplesBuffer(const SamplesBuffer &other) :
    channels(other.channels),
    frameLenght(other.frameLenght),
    samples(other.samples),
    rmsRunningSum(other.rmsRunningSum),
    rmsWindowSize(other.rmsWindowSize)
{
    // qWarning() << "Samples Buffer copy constructor!";
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
        return; //trying invert a non stereo buffer

    std::iter_swap(samples.begin(), samples.begin() + 1); // swap first and second channels
}

void SamplesBuffer::discardFirstSamples(unsigned int samplesToDiscard)
{
    int toDiscard = std::min(frameLenght, samplesToDiscard);
    int toCopy = frameLenght - toDiscard;
    uint newFrameLenght = frameLenght - toDiscard;
    for (uint c = 0; c < channels; ++c) {
        //std::rotate(samples[c].begin(), samples[c].begin() + toDiscard, samples[c].end());
         //samples[c].erase(samples[c].begin(), samples[c].begin() + toDiscard);
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
    if (channel > samples.size())
        channel = 0;
    return const_cast<float *>(&(samples[channel][0]));
}

void SamplesBuffer::applyGain(float gainFactor, float boostFactor)
{
    for (unsigned int c = 0; c < channels; ++c) {
        for (unsigned int i = 0; i < frameLenght; ++i)
            samples[c][i] *= (gainFactor * boostFactor);
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
    } else {
        applyGain(gainFactor, boostFactor);
    }
}

void SamplesBuffer::zero()
{
    for (unsigned int c = 0; c < channels; ++c)
        std::fill(samples[c].begin(), samples[c].end(), (float)0);
}

AudioPeak SamplesBuffer::computePeak()
{
    float abs; //max peak absolute value
    float maxPeaks[2] = {0};// left and right peaks
    for (unsigned int c = 0; c < channels; ++c) {
        float maxPeak = 0;
        for (unsigned int i = 0; i < frameLenght; ++i) {

            //max peak
            abs = std::fabs(samples[c][i]);
            if (abs > maxPeak)
                maxPeak = abs;
            maxPeaks[c] = maxPeak;

            //rms running squared sum
            squaredSums[c] += samples[c][i] * samples[c][i]; // squaring every sample and summing
        }
        summedSamples += frameLenght;
    }
    if (isMono()) {
        maxPeaks[1] = maxPeaks[0];
        squaredSums[1] = squaredSums[0];
    }

    //time to compute new rms values?
    if (summedSamples >= rmsWindowSize) {
        lastRmsValues[0] = std::sqrt(squaredSums[0]/summedSamples);
        lastRmsValues[1] = std::sqrt(squaredSums[1]/summedSamples);

        squaredSums[0] = squaredSums[1] = 0.0f; //reinitialize the rms running sum
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
    unsigned int framesToProcess = std::min((int)frameLenght, buffer.getFrameLenght());
    if (buffer.channels >= channels) {
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s)
                samples[c][s + internalWriteOffset] += buffer.samples[c][s];
        }
    } else {// samples is stereo and buffer is mono
        for (unsigned int s = 0; s < framesToProcess; ++s) {
            samples[0][s + internalWriteOffset] += buffer.samples[0][s];
            samples[1][s + internalWriteOffset] += buffer.samples[0][s];
        }
    }
}

void SamplesBuffer::add(unsigned int channel, float *samples, int samplesToAdd)
{
    if (channel < channels) {
        void *dest = &(this->samples[channel][0]);
        memcpy(dest, samples, std::min((int)frameLenght, samplesToAdd) * sizeof(float));
    } else {
        qWarning() << "wrong channel " << channel;
    }
}

void SamplesBuffer::add(int channel, int sampleIndex, float sampleValue)
{
    if (channelIsValid(channel) && sampleIndexIsValid(sampleIndex))
        samples[channel][sampleIndex] += sampleValue;
    else
        qWarning() << "channel ("<<channel<<") or sampleIndex ("<<sampleIndex<<") invalid";
}

void SamplesBuffer::set(int channel, int sampleIndex, float sampleValue)
{
    if (channelIsValid(channel) && sampleIndexIsValid(sampleIndex))
        samples[channel][sampleIndex] = sampleValue;
    else
        qWarning() << "channel ("<<channel<<") or sampleIndex ("<<sampleIndex<<") invalid";
}

int SamplesBuffer::getFrameLenght() const
{
    return this->frameLenght;
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

float SamplesBuffer::get(int channel, int sampleIndex) const
{
    if (!channelIsValid(channel) || !sampleIndexIsValid(sampleIndex))
        return 0;
    return samples[channel][sampleIndex ];
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
    if (buffer.channels <= 0 || channels <= 0)
        return;
    int framesToCopy = std::min(buffer.getFrameLenght(), (int)frameLenght);
    int channelsToProcess = std::min(channelsToCopy, std::min(buffer.getChannels(), (int)channels));
    if (channelsToProcess + bufferChannelOffset <= buffer.getChannels()) {// avoid invalid channel index
        int bytesToCopy = framesToCopy * sizeof(float);
        for (int c = 0; c < channelsToProcess; ++c)
            memcpy((void *)getSamplesArray(c), buffer.getSamplesArray(
                       c + bufferChannelOffset), bytesToCopy);
    }
}

void SamplesBuffer::set(const SamplesBuffer &buffer, unsigned int bufferOffset,
                        unsigned int samplesToCopy, unsigned int internalOffset)
{
    if (buffer.channels <= 0 || channels <= 0)
        return;

    unsigned int framesToProcess = std::min(samplesToCopy, buffer.getFrameLenght() - bufferOffset);
    if (framesToProcess > buffer.frameLenght)// não processa mais samples do que a quantidade existente em buffer
        framesToProcess = buffer.frameLenght;
    if ((int)(internalOffset + framesToProcess) > this->getFrameLenght())
        framesToProcess = (internalOffset + framesToProcess) - this->getFrameLenght();

    if (channels == buffer.channels) {// channels number are equal
        for (unsigned int c = 0; c < channels; ++c) {
            for (unsigned int s = 0; s < framesToProcess; ++s)
                samples[c][s + internalOffset] = buffer.samples[c][s + bufferOffset];
        }
    } else {// different number of channels
        if (!isMono()) {// copy every &buffer samples to LR in this buffer
            if (!buffer.isMono()) {
                int channelsToCopy = qMin(channels, buffer.channels);
                for (int c = 0; c < channelsToCopy; ++c) {
                    for (unsigned int s = 0; s < framesToProcess; ++s)
                        samples[c][s + internalOffset] = buffer.samples[c][s + bufferOffset];
                }
            } else {
                for (unsigned int s = 0; s < framesToProcess; ++s) {
                    samples[0][s + internalOffset] = buffer.samples[0][s + bufferOffset];
                    samples[1][s + internalOffset] = buffer.samples[0][s + bufferOffset];
                }
            }
        } else {// this buffer is mono, but the buffer in parameter is not! Mix down the stereo samples in one mono sample value.
            for (unsigned int s = 0; s < framesToProcess; ++s) {
                int index = s + bufferOffset;
                float v = (buffer.samples[0][index] + buffer.samples[1][index])/2.0f;
                samples[0][s + internalOffset] = v;
            }
        }
    }
}
