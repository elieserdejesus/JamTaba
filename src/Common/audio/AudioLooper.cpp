#include "AudioLooper.h"
#include <QDebug>

#include <memory>
#include <vector>
#include <QThread>

using namespace Audio;

/**
 * @brief The Looper::Layer class is an internal class to represent Looper layers.
 */

class Looper::Layer
{
public:

    Layer()
        : availableSamples(0)
    {
        //
    }

    void zero()
    {
        std::fill(leftChannel.begin(), leftChannel.end(), static_cast<float>(0));
        std::fill(rightChannel.begin(), rightChannel.end(), static_cast<float>(0));

        availableSamples = 0;
    }

    void append(const SamplesBuffer &samples, uint samplesToAppend)
    {
        const uint sizeInBytes = samplesToAppend * sizeof(float);

        std::memcpy(&(leftChannel[0]) + availableSamples, samples.getSamplesArray(0), sizeInBytes);
        const int secondChannelIndex = samples.isMono() ? 0 : 1;
        std::memcpy(&(rightChannel[0]) + availableSamples, samples.getSamplesArray(secondChannelIndex), sizeInBytes);

        availableSamples += samplesToAppend;
    }

    std::vector<float> getSamplesPeaks(uint samplesPerPeak) const
    {
        std::vector<float> maxPeaks;

        if (samplesPerPeak) {
            float lastMaxPeak = 0;
            for (uint i = 0; i < availableSamples; ++i) {
                float peak = qMax(qAbs(leftChannel[i]), qAbs(rightChannel[i]));
                if (peak > lastMaxPeak)
                    lastMaxPeak = peak;

                if (i % samplesPerPeak == 0) {
                    maxPeaks.push_back(lastMaxPeak);
                    lastMaxPeak = 0;
                }
            }
        }

        return maxPeaks;
    }

    void mixTo(SamplesBuffer &outBuffer, uint samplesToMix, uint intervalPosition)
    {
        if (samplesToMix > 0) {
            float* internalChannels[] = {&(leftChannel[0]), &(rightChannel[0])};
            const uint channels = outBuffer.getChannels();
            for (uint c = 0; c < channels; ++c) {
                float *out = outBuffer.getSamplesArray(c);
                for (uint s = 0; s < samplesToMix; ++s) {
                    out[s] += internalChannels[c][s + intervalPosition];
                }
            }
            availableSamples -= samplesToMix;
        }
    }

    void resize(quint32 samples)
    {
        if (samples > leftChannel.capacity())
            leftChannel.resize(samples);

        if (samples > rightChannel.capacity())
            rightChannel.resize(samples);
    }

    uint availableSamples;

private:
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

};

// ------------------------------------------------------------------------------------

Looper::Looper()
    : currentLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      state(LooperState::STOPPED)
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
    if (samplesInCycle != intervalLenght) {
        intervalLenght = samplesInCycle;
        for (Looper::Layer *layer : layers) {
            layer->resize(intervalLenght);
        }
    }

    intervalPosition = 0;

    LooperState previousState = state;

    if (state == LooperState::WAITING) {
        state = LooperState::RECORDING;
    }

    if (state == LooperState::RECORDING) {
        if (previousState == LooperState::WAITING)
            currentLayerIndex = 0; // start recording in first layer
        else
            currentLayerIndex = (currentLayerIndex + 1) % MAX_LOOP_LAYERS;

        layers[currentLayerIndex]->zero();
    }
}

void Looper::setActivated(bool activated)
{
    if (activated) {
        if (state == LooperState::STOPPED) {
            state = LooperState::WAITING;
        }
    }
    else {
        state = LooperState::STOPPED;
    }
}

const std::vector<float> Looper::getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const
{
    if (layerIndex < MAX_LOOP_LAYERS && state != LooperState::WAITING)
    {
        Audio::Looper::Layer *layer = layers[layerIndex];
        return layer->getSamplesPeaks(samplesPerPeak);
    }

    return std::vector<float>(); // empty vector
}

void Looper::process(SamplesBuffer &samples)
{
    if (state == LooperState::STOPPED || state == LooperState::WAITING)
        return;

    uint samplesToAppend = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);

    // store/rec current samples
    layers[currentLayerIndex]->append(samples, samplesToAppend);

    // render samples from previous interval layer
    Looper::Layer *loopLayer = getPreviousLayer();
    int samplesToMix= qMin(samplesToAppend, loopLayer->availableSamples);
    if (samplesToMix) {
        loopLayer->mixTo(samples, samplesToMix, intervalPosition); // mix buffered samples
    }

    intervalPosition = (intervalPosition + samplesToAppend) % intervalLenght;
}

Looper::Layer* Looper::getPreviousLayer() const
{
    if (currentLayerIndex > 0)
        return layers[currentLayerIndex - 1];

    return layers[MAX_LOOP_LAYERS - 1]; // current layer index is ZERO, returning the last layer
}
