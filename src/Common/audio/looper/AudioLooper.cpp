#include "AudioLooper.h"
#include "AudioLooperStates.h"
#include "AudioLooperLayer.h"

#include <QDebug>

#include <cstring>
#include <vector>
#include <QThread>

using namespace Audio;

Looper::Looper()
    : currentLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      maxLayers(4),
      state(new StoppedState()),
      mode(Mode::SEQUENCE),
      overdubbing(true),
      hearingOtherLayers(true)

{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) { // create all possible layers
        layers[l] = new LooperLayer();
    }
}

Looper::~Looper()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        delete layers[l];
    }
}

void Looper::incrementCurrentLayer()
{
    setCurrentLayer((currentLayerIndex + 1) % maxLayers);
}

void Looper::randomizeCurrentLayer()
{
    setCurrentLayer(qrand() % maxLayers);
}

void Looper::appendInCurrentLayer(const SamplesBuffer &samples, uint samplesToAppend)
{
     layers[currentLayerIndex]->append(samples, samplesToAppend);
}

void Looper::overdubInCurrentLayer(const SamplesBuffer &samples, uint samplesToMix)
{
    layers[currentLayerIndex]->overdub(samples, samplesToMix, intervalPosition);
}

void Looper::mixCurrentLayerTo(SamplesBuffer &samples, uint samplesToMix)
{
    mixLayer(currentLayerIndex, samples, samplesToMix, false);
}

bool Looper::currentLayerIsLocked() const
{
    return layers[currentLayerIndex]->isLocked();
}

void Looper::toggleLayerLockedState(quint8 layerIndex)
{
    setLayerLockedState(layerIndex, !(layers[layerIndex]->isLocked()));
}

bool Looper::canLockLayer(quint8 layer) const
{
    if (layer >= maxLayers)
        return false;

    return (isPlaying() || isStopped());// && layerIsValid(layer);
}

void Looper::setLayerLockedState(quint8 layerIndex, bool locked)
{
    if (canLockLayer(layerIndex)) {
        layers[layerIndex]->setLocked(locked);
        emit layerLockedStateChanged(layerIndex, locked);
    }
}

void Looper::setHearingOtherLayersWhileRecording(bool hearingOtherLayers)
{
    if (maxLayers > 1) {
        this->hearingOtherLayers = hearingOtherLayers;
    }
    else
        this->hearingOtherLayers = false;
}

bool Looper::layerIsLocked(quint8 layerIndex) const
{
    if (layerIndex < maxLayers)
        return layers[layerIndex]->isLocked();

    return false;
}

bool Looper::layerIsValid(quint8 layerIndex) const
{
    if (layerIndex < maxLayers)
        return layers[layerIndex]->isValid();

    return false;
}

void Looper::startRecording()
{
    int firstRecordingLayer = getFirstUnlockedLayerIndex(currentLayerIndex);
    if (firstRecordingLayer >= 0) {
        setCurrentLayer(firstRecordingLayer);
        layers[currentLayerIndex]->zero();
        setState(new RecordingState(this));
    }
}

void Looper::toggleRecording()
{

    if (isRecording() || isWaiting()) {
        if (mode != SELECTED_LAYER)
            stop();
        else
            play(); // in selected layer mode the looper will auto play when user finish recording
    }
    else {
        int firstRecordingLayer = getFirstUnlockedLayerIndex(currentLayerIndex);
        if (firstRecordingLayer >= 0) {
            setState(new WaitingState(this));
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
        play();
}

void Looper::stop()
{
    setState(new StoppedState());
}

void Looper::play()
{
    setState(new PlayingState(this));
}

bool Looper::canClearLayer(quint8 layer) const
{
    if (layer >= maxLayers)
        return false;

    return (isPlaying() || isStopped()) && !layerIsLocked(layer) && layerIsValid(layer);
}

void Looper::clearLayer(quint8 layer)
{
    if (canClearLayer(layer))
        layers[layer]->zero();
}

void Looper::clearCurrentLayer()
{
    clearLayer(currentLayerIndex);
}

void Looper::clearAllLayers()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        if (layers[l]) {
            layers[l]->zero();
        }
    }

    stop();
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

void Looper::setLayers(quint8 maxLayers)
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
        if (!layers[layer]->isLocked())
            return layer;

        layer = (layer + 1) % maxLayers;
        testedLayers++;
    }

    return -1;
}

int Looper::getNextUnlockedLayerIndex() const
{
    return getFirstUnlockedLayerIndex((currentLayerIndex + 1) % maxLayers);
}

void Looper::process(SamplesBuffer &samples)
{
    uint samplesToProcess = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);

    state->process(samples, samplesToProcess);

    // always update intervalPosition to keep the execution in sync when 'play' is pressed
    if (intervalLenght)
        intervalPosition = (intervalPosition + samplesToProcess) % intervalLenght;
}

void Looper::startNewCycle(uint samplesInCycle)
{
    if (samplesInCycle != intervalLenght)
        intervalLenght = samplesInCycle;

    intervalPosition = 0;

    const bool overdubbing = isOverdubbing();
    for (int l = 0; l < maxLayers; ++l)
        layers[l]->prepareForNewCycle(samplesInCycle, overdubbing);

    state->handleNewCycle(samplesInCycle);
}

void Looper::setState(LooperState *newState)
{
    if (state.data() != newState) {
        state.reset(newState);
        emit stateChanged();
    }
}


/**
 * @return True if we have at least one layer unlocked
 */
bool Looper::canRecord() const
{
    if (mode != Looper::SELECTED_LAYER)
        return getFirstUnlockedLayerIndex() >= 0;

    return !layers[currentLayerIndex]->isLocked(); // in SELECTED_LAYER_ONLY mode we can't allow recording in selected layer if this layer is locked
}

const std::vector<float> Looper::getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const
{
    if (layerIndex < maxLayers) {
        LooperLayer *layer = layers[layerIndex];
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

void Looper::mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix, bool replacing)
{
    if (layerIndex >= maxLayers)
        return;

    LooperLayer *loopLayer = layers[layerIndex];
    samplesToMix = qMin(samplesToMix, loopLayer->getAvailableSamples());
    if (samplesToMix) {
        loopLayer->mixTo(samples, samplesToMix, intervalPosition, replacing); // mix buffered samples
    }
}

void Looper::mixAllLayers(SamplesBuffer &samples, uint samplesToMix, int exceptLayer)
{
    for (uint layerIndex = 0; layerIndex < maxLayers; ++layerIndex) {
        if (layerIndex != exceptLayer) {
            const bool mixReplacing = layerIndex == currentLayerIndex;
            mixLayer(layerIndex, samples, samplesToMix, mixReplacing);
        }
    }
}

QString Looper::getModeString(Mode mode)
{
    switch (mode) {
        case Mode::SEQUENCE:        return tr("Sequence");
        case Mode::ALL_LAYERS:      return tr("All Layers");
        case Mode::RANDOM_LAYERS:   return tr("Random Layer");
        case SELECTED_LAYER:        return tr("Selected Layer");
    }

    return "Error";
}

bool Looper::isWaiting() const
{
    return state->isWaiting();
}

bool Looper::isPlaying() const
{
    return state->isPlaying();
}

bool Looper::isRecording() const
{
    return state->isRecording();
}

bool Looper::isStopped() const
{
    return state->isStopped();
}

bool Looper::currentModeHasRecordingProperties() const
{
    return mode == ALL_LAYERS || mode == SELECTED_LAYER;
}
