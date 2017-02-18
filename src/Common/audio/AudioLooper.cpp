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
        : availableSamples(0),
          lastSamplesPerPeak(0),
          lastCacheComputationSample(0)

    {
        //
    }

    ~Layer()
    {

    }

    void zero()
    {
        std::fill(leftChannel.begin(), leftChannel.end(), static_cast<float>(0));
        std::fill(rightChannel.begin(), rightChannel.end(), static_cast<float>(0));

        availableSamples = 0;
        lastSamplesPerPeak = 0;
        lastCacheComputationSample = 0;
        peaksCache.clear();
    }

    void append(const SamplesBuffer &samples, uint samplesToAppend)
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

    float computeMaxPeak(uint from, uint samplesPerPeak) const
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

    std::vector<float> getSamplesPeaks(uint samplesPerPeak)
    {
        if (lastSamplesPerPeak == samplesPerPeak) { // cache hit?
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

    std::vector<float> peaksCache;
    uint lastSamplesPerPeak;
    uint lastCacheComputationSample;
};

// ------------------------------------------------------------------------------------

Looper::Looper()
    : currentLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      maxLayers(4),
      state(LooperState::STOPPED),
      playMode(PlayMode::SEQUENCE)
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) { // create all possible layers
        layers[l] = new Looper::Layer();
    }
}

Looper::~Looper()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        delete layers[l];
    }
}

void Looper::stop()
{
    setState(Looper::STOPPED);
}

void Looper::clearSelectedLayer()
{
    if (isPlaying()) {
        quint8 layerIndex = currentLayerIndex;
        if (layerIndex < maxLayers) {
            layers[layerIndex]->zero();
        }
    }
}

void Looper::clearAllLayers()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        if (layers[l]) {
            layers[l]->zero();
        }
    }

    setState(LooperState::STOPPED);
}

void Looper::selectLayer(quint8 layerIndex)
{
    if (isWaiting() || isRecording()) // can't select layer in these states
        return;

    if (layerIndex < maxLayers) {
        currentLayerIndex = layerIndex;
    }
}

void Looper::setMaxLayers(quint8 maxLayers)
{
    if (maxLayers < 1) {
        maxLayers = 1;
    }
    else if (maxLayers > MAX_LOOP_LAYERS) {
        maxLayers = MAX_LOOP_LAYERS;
    }

    this->maxLayers = maxLayers;

    emit maxLayersChanged(maxLayers);
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

    if (isWaiting()) {
        currentLayerIndex = 0; // always start recording in first layer
        layers[currentLayerIndex]->zero();
        setState(LooperState::RECORDING);
        return;
    }

    if (isPlaying()) {
        if (playMode == PlayMode::RANDOM_LAYERS)
            currentLayerIndex = qrand() % maxLayers;
        else if (playMode == PlayMode::SEQUENCE)
            currentLayerIndex = (currentLayerIndex + 1) % maxLayers;

        return;  // ALL_LAYERS and SELECTED_LAYER_ONLY play states are not touching in currentLayerIndex
    }

    if (isRecording()) {
        currentLayerIndex = (currentLayerIndex + 1) % maxLayers; // advance record to next layer
        if (currentLayerIndex == 0) // stop recording when backing to first layer
            setState(LooperState::PLAYING);
        else
            layers[currentLayerIndex]->zero(); // zero current layer if keep recording
    }
}

void Looper::setState(LooperState state)
{
    if (this->state != state) {
        this->state = state;
        emit stateChanged();
    }
}


/**
 * @return True if any layer has enough available samples
 */
bool Looper::canPlay(uint intervalLenght) const
{
    for (int l = 0; l < maxLayers; ++l) {
        if (layers[l]->availableSamples >= intervalLenght) {
            return true;
        }
    }

    return false;
}

const std::vector<float> Looper::getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const
{
    if (layerIndex < maxLayers && !isWaiting()) {
        Audio::Looper::Layer *layer = layers[layerIndex];
        return layer->getSamplesPeaks(samplesPerPeak);
    }

    return std::vector<float>(); // empty vector
}

void Looper::process(SamplesBuffer &samples)
{
    bool canProcess = isRecording() || isPlaying();
    uint samplesToProcess = samples.getFrameLenght();

    if (canProcess) {
        samplesToProcess = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);

        if (isRecording()) { // store/rec current samples
            layers[currentLayerIndex]->append(samples, samplesToProcess);
        }
        else if (isPlaying()) {
            switch (playMode) {
            case PlayMode::SEQUENCE:
            case PlayMode::RANDOM_LAYERS: // layer index in randomized in startNewCycle() function
            case PlayMode::SELECTED_LAYER_ONLY:
                mixLayer(currentLayerIndex, samples, samplesToProcess);
                break;
            case PlayMode::ALL_LAYERS:
                playAllLayers(samples, samplesToProcess);
                break;
            default:
                qCritical() << playMode << " not implemented yet!";
            }
        }
    }

    // always update intervalPosition to keep the execution in sync when 'play' is pressed
    if (intervalLenght)
        intervalPosition = (intervalPosition + samplesToProcess) % intervalLenght;
}

/**
 Play one layer per interval
 */
void Looper::mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix)
{
    if (layerIndex >= maxLayers)
        return;

    Looper::Layer *loopLayer = layers[layerIndex];
    samplesToMix = qMin(samplesToMix, loopLayer->availableSamples);
    if (samplesToMix) {
        loopLayer->mixTo(samples, samplesToMix, intervalPosition); // mix buffered samples
    }
}

void Looper::playAllLayers(SamplesBuffer &samples, uint samplesToMix)
{
    for (int l = 0; l < maxLayers; ++l) {
        mixLayer(l, samples, samplesToMix);
    }
}

QString Looper::getPlayModeString(PlayMode playMode)
{
    switch (playMode) {
        case PlayMode::SEQUENCE:        return tr("Sequence");
        case PlayMode::ALL_LAYERS:      return tr("All Layers");
        case PlayMode::RANDOM_LAYERS:   return tr("Random");
        case SELECTED_LAYER_ONLY:       return tr("Selected");
    }

    return "Error";
}

