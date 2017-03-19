#include "Looper.h"
#include "LooperStates.h"
#include "LooperLayer.h"

#include <QDebug>

#include <cstring>
#include <vector>
#include <QThread>

using namespace Audio;

Looper::Looper()
    : currentLayerIndex(0),
      focusedLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      changed(false),
      maxLayers(4),
      state(new StoppedState()),
      mode(Mode::Sequence),
      resetRequested(false),
      newMaxLayersRequested(0)
{
    initialize();
}

Looper::Looper(Looper::Mode initialMode, quint8 maxLayers)
    : currentLayerIndex(0),
      focusedLayerIndex(0),
      intervalLenght(0),
      intervalPosition(0),
      changed(false),
      maxLayers(maxLayers),
      state(new StoppedState()),
      mode(initialMode),
      resetRequested(false),
      newMaxLayersRequested(0)
{
    initialize();
}

void Looper::nextMuteState(quint8 layer) // called when 'mute button' is clicked
{
    bool canMute = layer < maxLayers && mode == Looper::AllLayers;
    if (canMute) {
        const LooperLayer::MuteState currentState = layers[layer]->getMuteState();
        LooperLayer::MuteState newMuteState = currentState;

        switch (currentState) {
        case LooperLayer::Unmuted:
            newMuteState = isPlaying() ? LooperLayer::WaitingToMute : LooperLayer::Muted;
            break;
        case LooperLayer::Muted:
            newMuteState = isPlaying() ? LooperLayer::WaitingToUnmute : LooperLayer::Unmuted;
            break;
        case LooperLayer::WaitingToMute:
            newMuteState = LooperLayer::Muted;
            break;
        case LooperLayer::WaitingToUnmute:
            newMuteState = LooperLayer::Unmuted;
            break;
        }

        layers[layer]->setMuteState(newMuteState);
        emit layerMuteStateChanged(layer, static_cast<quint8>(newMuteState));
    }
}

float Looper::getLayerGain(quint8 layer) const
{
    if (layer < maxLayers)
        return layers[layer]->getGain();

    return 1.0;
}

float Looper::getLayerPan(quint8 layer) const
{
    if (layer < maxLayers)
        return layers[layer]->getPan();

    return 0.0;
}


AudioPeak Looper::getLastPeak() const
{
    return lastPeak;
}

bool Looper::isFull() const
{
    for (uint l = 0; l < maxLayers; ++l) {
        if (!layerIsValid(l))
            return false;
    }

    return true;
}

bool Looper::isEmpty() const
{
    return getLastValidLayer() < 0;
}

int Looper::getLastValidLayer() const
{
    for (int l = maxLayers - 1; l >= 0; --l) {
        if (layerIsValid(l))
            return l;
    }

    return -1; // no valid layers, all layers are empty
}

void Looper::initialize()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) { // create all possible layers
        layers[l] = new LooperLayer();
    }

    Looper::Mode modes[] = {Looper::Sequence, Looper::AllLayers, Looper::SelectedLayer};
    for (Looper::Mode mode : modes) {
        modeOptions[mode].recordingOptions = getDefaultSupportedRecordingOptions(mode);
        modeOptions[mode].playingOptions = getDefaultSupportedPlayingOptions(mode);
    }
}


void Looper::reset()
{
    resetRequested = true;
    changed = false;

    lastPeak.zero();
}

Looper::~Looper()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        if (layers[l])
            delete layers[l];

        layers[l] = nullptr;
    }
}

void Looper::setLayerGain(quint8 layerIndex, float gain)
{
    if (layerIndex < maxLayers) {
        layers[layerIndex]->setGain(gain);
        setChanged(true);
        emit layerChanged(layerIndex);
    }
}

void Looper::setLayerPan(quint8 layerIndex, float pan)
{
    if (layerIndex < maxLayers) {
        layers[layerIndex]->setPan(pan);
        setChanged(true);
        emit layerChanged(layerIndex);
    }
}

uint Looper::getLockedLayers() const
{
    uint lockedLayers = 0;
    for (int l = 0; l < maxLayers; ++l) {
        if (layers[l]->isLocked())
            lockedLayers++;
    }
    return lockedLayers;
}

void Looper::incrementLockedLayer()
{
    quint8 nextLayer = currentLayerIndex;
    bool isRandomizing = getOption(Looper::RandomizeLayers);
    if (getLockedLayers() > 0) {
        do {
            if (isRandomizing)
                nextLayer = qrand() % maxLayers;
            else
                nextLayer = (nextLayer + 1) % maxLayers;
        }
        while(!layerIsLocked(nextLayer));
    }

    setCurrentLayer(nextLayer);
}

void Looper::incrementCurrentLayer()
{
    bool isPlayingLockedLayersOnly = getOption(Looper::PlayLockedLayers);
    bool isRandomizing = getOption(Looper::RandomizeLayers);

    if (isPlayingLockedLayersOnly) {
        incrementLockedLayer();
        return;
    }

    quint8 nextLayer = currentLayerIndex;
    if (isRandomizing && maxLayers > 1)
        nextLayer = qrand() % maxLayers;
    else
        nextLayer = (nextLayer + 1) % maxLayers;

    setCurrentLayer(nextLayer);
}

void Looper::appendInCurrentLayer(const SamplesBuffer &samples, uint samplesToAppend)
{
     layers[currentLayerIndex]->append(samples, samplesToAppend);
     changed = true;
}

void Looper::overdubInCurrentLayer(const SamplesBuffer &samples, uint samplesToMix)
{
    layers[currentLayerIndex]->overdub(samples, samplesToMix, intervalPosition);
    changed = true;
}

void Looper::mixCurrentLayerTo(SamplesBuffer &samples, uint samplesToMix)
{
    mixLayer(currentLayerIndex, samples, samplesToMix);
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

    return (isPlaying() || isStopped());
}

void Looper::setLayerLockedState(quint8 layerIndex, bool locked)
{
    if (canLockLayer(layerIndex)) {
        layers[layerIndex]->setLocked(locked);

        if (focusedLayerIndex == layerIndex)
            focusedLayerIndex = -1; // locked layer can't be focused

        changed = true;

        emit layerChanged(layerIndex);
    }
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
    quint8 startFrom = (focusedLayerIndex >= 0) ? focusedLayerIndex : 0;
    int firstRecordingLayer = getFirstUnlockedLayerIndex(startFrom);
    if (firstRecordingLayer >= 0) {
        setCurrentLayer(firstRecordingLayer);
        focusedLayerIndex = firstRecordingLayer;

        bool isOverdubbing = getOption(Looper::Overdub);
        if (!isOverdubbing) // avoid discard layer content if is overdubbing
            layers[currentLayerIndex]->zero();

        setState(new RecordingState(this, firstRecordingLayer));
    }
}

void Looper::toggleRecording()
{
    if (isRecording() || isWaiting()) {
        play(); // auto play when recording is finished (rec button is pressed)
    }
    else {
        quint8 startFrom = (focusedLayerIndex >= 0) ? focusedLayerIndex : 0;
        int firstRecordingLayer = getFirstUnlockedLayerIndex(startFrom);
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
    if (canClearLayer(layer)) {
        layers[layer]->zero();
        setChanged(true);
    }
}

void Looper::clearCurrentLayer()
{
    clearLayer(currentLayerIndex);
}

bool Looper::canSelectLayers() const
{
    if (maxLayers <= 1)
        return false;

    if (isRecording() || isWaiting()) // can't select layer while recording or waiting
        return false;

    if (getLockedLayers() >= maxLayers) { // all layers locked, can't select any layer
        return false;
    }

    return true;
}

void Looper::selectLayer(quint8 layerIndex)
{
    if (!canSelectLayers())
        return;

    if (!layerIsLocked(layerIndex))
        focusedLayerIndex = layerIndex;

    if (mode == Looper::SelectedLayer)
        setCurrentLayer(layerIndex);
}

void Looper::setCurrentLayer(quint8 newLayer)
{
    if (newLayer < maxLayers) {
        currentLayerIndex = newLayer;
        if (isRecording())
            focusedLayerIndex = newLayer;

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

    newMaxLayersRequested = maxLayers; // schedule the max layer change to next process
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

void Looper::addBuffer(const SamplesBuffer &samples)
{
    uint samplesToProcess = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);
    state->addBuffer(samples, samplesToProcess);
}

void Looper::mixToBuffer(SamplesBuffer &samples)
{
    uint samplesToProcess = qMin(samples.getFrameLenght(), intervalLenght - intervalPosition);
    AudioPeak peakBeforeMix = samples.computePeak();
    state->mixTo(samples, samplesToProcess);
    AudioPeak peakAfterMix = samples.computePeak();

    // always update intervalPosition to keep the execution in sync when 'play' is pressed
    if (intervalLenght)
        intervalPosition = (intervalPosition + samplesToProcess) % intervalLenght;

    processChangeRequests();

    lastPeak = peakAfterMix - peakBeforeMix; // minus operator is overloaded in AudioPeak class
}

void Looper::processChangeRequests()
{
    // reset requested in last process cycle?
    if (resetRequested && !isRecording()) {
        for (uint l = 0; l < maxLayers; ++l) {
            layers[l]->reset();
        }
        resetRequested = false;
    }

    // max layers change requested?
    if (newMaxLayersRequested) {
        this->maxLayers = newMaxLayersRequested;
        newMaxLayersRequested = 0;

        if (focusedLayerIndex >= maxLayers) // focused layer is not valid in new maxLayers?
            focusedLayerIndex = -1;

        if (currentLayerIndex >= maxLayers) { // currentLayer is not valid in new maxLayers?
            if (mode == Looper::Mode::Sequence) {
                incrementCurrentLayer();
            }
            else {
                setCurrentLayer(0);
            }
        }

        emit maxLayersChanged(maxLayers);
    }
}

void Looper::startNewCycle(uint samplesInCycle)
{
    if (samplesInCycle != intervalLenght)
        intervalLenght = samplesInCycle;

    intervalPosition = 0;

    bool isOverdubbing = getOption(Looper::Overdub);
    for (quint8 l = 0; l < MAX_LOOP_LAYERS; ++l) {
        layers[l]->prepareForNewCycle(samplesInCycle, isOverdubbing);

        bool canMute = l < maxLayers && mode == Looper::AllLayers;
        if (canMute) {
            LooperLayer::MuteState currentMuteState = layers[l]->getMuteState();
            if (currentMuteState == LooperLayer::WaitingToMute || currentMuteState == LooperLayer::WaitingToUnmute)
                nextMuteState(l);
        }
    }

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
    if (mode != Looper::SelectedLayer)
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

        for (quint8 l = 0; l < maxLayers; ++l)  // reset mute state in all layers when mode is changed
            layers[l]->setMuteState(LooperLayer::Unmuted);

        emit modeChanged();
    }
}

void Looper::mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix)
{
    if (layerIndex >= maxLayers)
        return;

    LooperLayer *loopLayer = layers[layerIndex];
    samplesToMix = qMin(samplesToMix, loopLayer->getAvailableSamples());
    if (samplesToMix) {
        loopLayer->mixTo(samples, samplesToMix, intervalPosition); // mix buffered samples
    }
}

void Looper::mixAllLayers(SamplesBuffer &samples, uint samplesToMix)
{
    for (uint layer = 0; layer < maxLayers; ++layer)
        mixLayer(layer, samples, samplesToMix);
}

void Looper::mixLockedLayers(SamplesBuffer &samples, uint samplesToMix)
{
    for (uint layer = 0; layer < maxLayers; ++layer) {
        if (layerIsLocked(layer)) {
            mixLayer(layer, samples, samplesToMix);
        }
    }
}

QString Looper::getModeString(Mode mode)
{
    switch (mode) {
        case Mode::Sequence:        return tr("Sequence");
        case Mode::AllLayers:      return tr("All Layers");
        case SelectedLayer:        return tr("Selected Layer");
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

QMap<Looper::RecordingOption, bool> Looper::getDefaultSupportedRecordingOptions(Looper::Mode mode)
{
    QMap<Looper::RecordingOption, bool> options;
    options[Looper::Overdub] = false;

    if (mode == Looper::Sequence) {
        options[Looper::HearAllLayers] = false;
    }

    return options;
}

QMap<Looper::PlayingOption, bool> Looper::getDefaultSupportedPlayingOptions(Looper::Mode mode)
{
    QMap<Looper::PlayingOption, bool> options;

    if (mode == Looper::Sequence) {
        options[Looper::PlayLockedLayers] = false;
        options[Looper::RandomizeLayers] = false;
        options[Looper::PlayNonEmptyLayers] = true;
    }

    if (mode == Looper::AllLayers) {
        options[Looper::PlayLockedLayers] = false;
        options[Looper::PlayNonEmptyLayers] = true;
    }

    return options;
}

QList<SamplesBuffer> Looper::getLayersSamples() const
{
    QList<SamplesBuffer> layersList;
    for (uint layer = 0; layer < maxLayers; ++layer) {
        if (layerIsValid(layer)) { // not empty?
            layersList.append(layers[layer]->getAllSamples());
        }
    }

    return layersList;
}

void Looper::setChanged(bool changed)
{
    this->changed = changed;
}

bool Looper::canSave() const
{
    if (!changed || isWaiting() || isRecording())
        return false;

    for (int l = 0; l < maxLayers; ++l) {
        if (layers[l]->isValid())
            return true;
    }

    return false;
}

void Looper::setLayerSamples(quint8 layer, const SamplesBuffer &samples)
{
    if (layer < maxLayers) {
        layers[layer]->setSamples(samples);
    }
}
