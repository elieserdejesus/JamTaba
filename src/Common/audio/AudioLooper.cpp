#include "AudioLooper.h"
#include <QDebug>

#include <cstring>
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
          lastCacheComputationSample(0),
          locked(false)

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

    void overdub(const SamplesBuffer &samples, uint samplesToMix, uint startPosition)
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

        // build peaks cache
        if (lastSamplesPerPeak) {
            if (availableSamples - lastCacheComputationSample >= lastSamplesPerPeak) { // enough samples to cache a new max peak?
                const int position = (availableSamples/lastSamplesPerPeak) - 1;
                float lastPeak = computeMaxPeak(lastCacheComputationSample, lastSamplesPerPeak);
                if (position >= 0 && position < peaksCache.size())
                    peaksCache[position] = lastPeak;
                else
                    peaksCache.push_back(lastPeak);
                lastCacheComputationSample += lastSamplesPerPeak;
            }
        }
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

    void setLocked(bool locked)
    {
        this->locked = locked;
    }

    uint availableSamples;
    uint lastCacheComputationSample;
    bool locked;

private:
    std::vector<float> leftChannel;
    std::vector<float> rightChannel;

    std::vector<float> peaksCache;
    uint lastSamplesPerPeak;


};

// ------------------------------------------------------------------------------------

Looper::Looper()
    : currentLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      maxLayers(4),
      state(LooperState::STOPPED),
      mode(Mode::SEQUENCE),
      hearingOtherLayersWhileRecording(true),
      overdubbing(true)
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

void Looper::toggleLayerLockedState(quint8 layerIndex)
{
    setLayerLockedState(layerIndex, !(layers[layerIndex]->locked));
}

void Looper::setLayerLockedState(quint8 layerIndex, bool locked)
{
    bool canLockLayers = isPlaying() || isStopped();
    if (layerIndex < maxLayers && canLockLayers) {
        layers[layerIndex]->setLocked(locked);
        emit layerLockedStateChanged(layerIndex, locked);
    }
}

void Looper::setHearingOtherLayersWhileRecording(bool hearingOtherLayers)
{
    this->hearingOtherLayersWhileRecording = hearingOtherLayers;
}

bool Looper::layerIsLocked(quint8 layerIndex) const
{
    if (layerIndex < maxLayers)
        return layers[layerIndex]->locked;

    return false;
}

bool Looper::layerIsValid(quint8 layerIndex) const
{
    if (layerIndex < maxLayers)
        return layers[layerIndex]->availableSamples > 0;

    return false;
}

void Looper::toggleRecording()
{
    if (isRecording() || isWaiting()) {
        if (mode != SELECTED_LAYER_ONLY)
            stop();
        else
            setState(Looper::PLAYING); // in selected layer mode the looper will auto play when user finish recording
    }
    else {
        int firstRecordingLayer = getFirstUnlockedLayerIndex(currentLayerIndex);
        if (firstRecordingLayer >= 0) {
            setState(Looper::WAITING);
            setCurrentLayer(firstRecordingLayer);
        }
        else {
            stop();
        }
    }
}

void Looper::togglePlay()
{
    if (isPlaying())
        stop();
    else
        setState(Looper::PLAYING);
}

void Looper::stop()
{
    setState(Looper::STOPPED);
}

void Looper::clearSelectedLayer()
{
    bool canClearSelectedLayer = (isPlaying() || isStopped()) && !layerIsLocked(currentLayerIndex);
    if (canClearSelectedLayer) {
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
    if (isRecording()) // can't select layer while recording
        return;

    setCurrentLayer(layerIndex);
}

void Looper::setCurrentLayer(quint8 newLayer)
{
    if (newLayer < maxLayers) {
        currentLayerIndex = newLayer;
        emit currentLayerChanged(newLayer);
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

int Looper::getFirstUnlockedLayerIndex(quint8 startingFrom) const
{
    quint8 testedLayers = 0;
    quint8 layer = startingFrom % maxLayers;

    while (testedLayers < maxLayers) {
        if (!layers[layer]->locked)
            return layer;

        layer = (layer + 1) % maxLayers;
        testedLayers++;
    }

    return -1;
}

void Looper::process(SamplesBuffer &samples)
{
    const bool canProcess = isRecording() || isPlaying();
    uint samplesToProcess = samples.getFrameLenght();

    if (canProcess) {
        samplesToProcess = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);

        if (isRecording() && !layerIsLocked(currentLayerIndex)) { // store/rec current samples, but avoid recording in locked layers

            if (mode != SELECTED_LAYER_ONLY || (mode == SELECTED_LAYER_ONLY && !overdubbing))
                layers[currentLayerIndex]->append(samples, samplesToProcess);
            else
                layers[currentLayerIndex]->overdub(samples, samplesToProcess, intervalPosition);

            bool hearingLayersWhileRecording = (mode == ALL_LAYERS && hearingOtherLayersWhileRecording) || (mode == SELECTED_LAYER_ONLY && overdubbing);
            if (hearingLayersWhileRecording) {
                const quint8 excludeLayer = mode == SELECTED_LAYER_ONLY ? -1 : currentLayerIndex;
                mixAllLayers(samples, samplesToProcess, excludeLayer); // user can hear other layers while recording
            }
        }
        else if (isPlaying()) {
            switch (mode) {
            case Mode::SEQUENCE:
            case Mode::RANDOM_LAYERS: // layer index in randomized in startNewCycle() function
            case Mode::SELECTED_LAYER_ONLY:
                mixLayer(currentLayerIndex, samples, samplesToProcess);
                break;
            case Mode::ALL_LAYERS:
                mixAllLayers(samples, samplesToProcess, -1); // mix all layers, no excluded layers
                break;
            default:
                qCritical() << mode << " not implemented yet!";
            }
        }
    }

    // always update intervalPosition to keep the execution in sync when 'play' is pressed
    if (intervalLenght)
        intervalPosition = (intervalPosition + samplesToProcess) % intervalLenght;
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
        int firstRecordingLayer = getFirstUnlockedLayerIndex(currentLayerIndex);
        if (firstRecordingLayer >= 0) {
            setCurrentLayer(firstRecordingLayer);
            layers[currentLayerIndex]->zero();
            setState(LooperState::RECORDING);
        }
        return;
    }

    if (isPlaying()) {
        if (mode == Mode::RANDOM_LAYERS)
            setCurrentLayer(qrand() % maxLayers);
        else if (mode == Mode::SEQUENCE)
            setCurrentLayer((currentLayerIndex + 1) % maxLayers);

        return;  // ALL_LAYERS and SELECTED_LAYER_ONLY play states are not touching in currentLayerIndex
    }

    if (isRecording()) {
        if (mode != SELECTED_LAYER_ONLY) {
            quint8 nextLayer = (currentLayerIndex + 1) % maxLayers;
            while (layers[nextLayer]->locked && nextLayer > 0) { // exit from loop if reach 0
                nextLayer = (nextLayer + 1) % maxLayers;
            }

            if (nextLayer == 0 || layers[nextLayer]->locked) { // stop recording when backing to first layer
                stop();
                setState(LooperState::PLAYING);
                setCurrentLayer(nextLayer);
            }
            else {
                setCurrentLayer(nextLayer); // advance record to next layer
                layers[currentLayerIndex]->zero(); // zero current layer if keep recording
            }
        }
        else { // SELECTED_LAYER_ONLY when recording
            if (isOverdubbing()) {
                for (uint l = 0; l < maxLayers; ++l) {
                    layers[l]->availableSamples = 0; // this is necessary to rebuild the peaks when overdubbing
                    layers[l]->lastCacheComputationSample = 0;
                }
            }
            else { // if not overdubbing we need auto stop recording
                stop();
                setState(LooperState::PLAYING);
            }
        }
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
 * @return True if we have at least one layer unlocked
 */
bool Looper::canRecord() const
{
    if (mode != Looper::SELECTED_LAYER_ONLY)
        return getFirstUnlockedLayerIndex() >= 0;

    return !layers[currentLayerIndex]->locked; // in SELECTED_LAYER_ONLY mode we can't allow recording in selected layer if this layer is locked
}

const std::vector<float> Looper::getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const
{
    if (layerIndex < maxLayers) {
        Audio::Looper::Layer *layer = layers[layerIndex];
        return layer->getSamplesPeaks(samplesPerPeak);
    }

    return std::vector<float>(); // empty vector
}

void Looper::setMode(Mode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        emit modeChanged();
    }
}

void Looper::setOverdubbing(bool overdubbing)
{
    this->overdubbing = overdubbing;
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

void Looper::mixAllLayers(SamplesBuffer &samples, uint samplesToMix, int exceptLayer)
{
    for (uint layerIndex = 0; layerIndex < maxLayers; ++layerIndex) {
        if (layerIndex != exceptLayer)
            mixLayer(layerIndex, samples, samplesToMix);
    }
}

QString Looper::getModeString(Mode mode)
{
    switch (mode) {
        case Mode::SEQUENCE:        return tr("Sequence");
        case Mode::ALL_LAYERS:      return tr("All Layers");
        case Mode::RANDOM_LAYERS:   return tr("Random");
        case SELECTED_LAYER_ONLY:       return tr("Selected");
    }

    return "Error";
}

