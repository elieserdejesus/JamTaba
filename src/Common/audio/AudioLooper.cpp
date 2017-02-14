#include "AudioLooper.h"
#include <QDebug>

#include <memory>
#include <vector>

using namespace Audio;

/**
 * @brief The Looper::Layer class is an internal class to represent Looper layers.
 */

class Looper::Layer
{
public:

    Layer()
        : availableSamples(0),
          cycleLenght(0)
    {
        //
    }

    void zero()
    {
        std::fill(leftChannel.begin(), leftChannel.end(), static_cast<float>(0));
        std::fill(rightChannel.begin(), rightChannel.end(), static_cast<float>(0));

        availableSamples = 0;
    }

    void append(const SamplesBuffer &samples)
    {
        const uint freeSpace = cycleLenght - availableSamples;
        const uint samplesToAppend = qMin(samples.getFrameLenght(), freeSpace);
        const uint sizeInBytes = samplesToAppend * sizeof(float);

        if (freeSpace) {

            std::memcpy(&(leftChannel[0]) + availableSamples, samples.getSamplesArray(0), sizeInBytes);
            const int secondChannelIndex = samples.isMono() ? 0 : 1;
            std::memcpy(&(rightChannel[0]) + availableSamples, samples.getSamplesArray(secondChannelIndex), sizeInBytes);

            availableSamples += samplesToAppend;
        }
    }

    void mixTo(SamplesBuffer &outBuffer)
    {
        const int samplesToMix = qMin(outBuffer.getFrameLenght(), availableSamples);
        if (samplesToMix > 0) {
            uint readOffset = cycleLenght - availableSamples;
            float* internalChannels[] = {&(leftChannel[0]), &(rightChannel[0])};
            const uint channels = outBuffer.getChannels();
            for (uint c = 0; c < channels; ++c) {
                float *out = outBuffer.getSamplesArray(c);
                for (int s = 0; s < samplesToMix; ++s) {
                    out[s] += internalChannels[c][s + readOffset];
                }
            }
            availableSamples -= samplesToMix;
        }
    }

    void setCycleLenghtInSamples(quint32 samples)
    {
        if (samples > leftChannel.capacity())
            leftChannel.resize(samples);

        if (samples > rightChannel.capacity())
            rightChannel.resize(samples);

        this->cycleLenght = samples;
    }

    uint availableSamples;
    uint cycleLenght;

private:
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

};

// ------------------------------------------------------------------------------------

Looper::Looper()
    : playingBufferedSamples(false),
      currentLayerIndex(0)
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        layers[l] = new Looper::Layer();
    }
}

Looper::~Looper()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        delete layers[l];
    }
}

void Looper::startNewCycle(uint samplesInCycle)
{
    currentLayerIndex = (currentLayerIndex + 1) % MAX_LOOP_LAYERS; // pointing to next layer

    layers[currentLayerIndex]->setCycleLenghtInSamples(samplesInCycle);
    layers[currentLayerIndex]->zero(); // clear the new current layer before start buffering loop samples
}

void Looper::playBufferedSamples(bool playBufferedSamples) {
    this->playingBufferedSamples = playBufferedSamples;
}

void Looper::process(SamplesBuffer &samples)
{
    // store/rec current samples
    layers[currentLayerIndex]->append(samples);

    Audio::AudioPeak peak = samples.computePeak();
    emit bufferedSamplesPeakAvailable(peak.getMaxPeak(), samples.getFrameLenght(), currentLayerIndex);

    if (!playingBufferedSamples) {
        return;
    }

    // render samples from previous interval
    quint8 previousLayer = getPreviousLayerIndex();
    Looper::Layer *loopLayer = layers[previousLayer];

    int samplesToProcess = qMin(samples.getFrameLenght(), loopLayer->availableSamples);
    if (samplesToProcess) {
        loopLayer->mixTo(samples); // mix buffered samples
    }
}

quint8 Looper::getPreviousLayerIndex() const
{
    if (currentLayerIndex > 0)
        return currentLayerIndex - 1;

    return MAX_LOOP_LAYERS - 1; // current layer is ZERO, returning the last layer index
}
