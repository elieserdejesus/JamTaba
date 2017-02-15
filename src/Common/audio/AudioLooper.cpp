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
    : playingBufferedSamples(false),
      currentLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0)
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        layers[l] = new Looper::Layer();
    }

    playBufferedSamples(true); // TEST ONLY
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

    currentLayerIndex = (currentLayerIndex + 1) % MAX_LOOP_LAYERS;
    layers[currentLayerIndex]->zero();
}

void Looper::playBufferedSamples(bool playBufferedSamples) {
    this->playingBufferedSamples = playBufferedSamples;
}

void Looper::process(SamplesBuffer &samples)
{
    if (!intervalLenght)
        return;

    uint samplesToAppend = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);

    // store/rec current samples
    layers[currentLayerIndex]->append(samples, samplesToAppend);

    Audio::AudioPeak peak = samples.computePeak();
    emit bufferedSamplesPeakAvailable(peak.getMaxPeak(), samplesToAppend, currentLayerIndex);

    if (!playingBufferedSamples) {
        qDebug() << "not playing";
        return;
    }

    // render samples from previous interval
    quint8 previousLayer = getPreviousLayerIndex();
    Looper::Layer *loopLayer = layers[previousLayer];

    int samplesToMix= qMin(samplesToAppend, loopLayer->availableSamples);
    if (samplesToMix) {
        loopLayer->mixTo(samples, samplesToMix, intervalPosition); // mix buffered samples
    }

    intervalPosition = (intervalPosition + samplesToAppend) % intervalLenght;
}

quint8 Looper::getPreviousLayerIndex() const
{
    if (currentLayerIndex > 0)
        return currentLayerIndex - 1;

    return MAX_LOOP_LAYERS - 1; // current layer is ZERO, returning the last layer index
}
