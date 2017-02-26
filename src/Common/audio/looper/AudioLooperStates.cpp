#include "AudioLooperStates.h"
#include "AudioLooper.h"
#include "AudioLooperLayer.h"

#include <QDebug>

using namespace Audio;

// ----------------------------------------------------

LooperState::LooperState(Looper *looper)
    : looper(looper)
{

}

// ------------------------------------------------


StoppedState::StoppedState()
    : LooperState(nullptr)
{

}

void StoppedState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)
}

void StoppedState::process(SamplesBuffer &samples, uint samplesToProcess)
{
    Q_UNUSED(samples)
    Q_UNUSED(samplesToProcess)
}

// -------------------------------------------------------------------

PlayingState::PlayingState(Looper *looper)
    : LooperState(looper)
{

}

void PlayingState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)

    if (looper->mode == Looper::Mode::RANDOM_LAYERS)
        looper->randomizeCurrentLayer();
    else if (looper->mode == Looper::Mode::SEQUENCE)
        looper->incrementCurrentLayer();

    // ALL_LAYERS and SELECTED_LAYER_ONLY play states are not touching in currentLayerIndex
}

void PlayingState::process(SamplesBuffer &samples, uint samplesToProcess)
{
    switch (looper->mode) {
    case Looper::SEQUENCE:
    case Looper::RANDOM_LAYERS: // layer index is randomized in handleNewCycle()
    case Looper::SELECTED_LAYER:
        looper->mixCurrentLayerTo(samples, samplesToProcess);
        break;
    case Looper::ALL_LAYERS:
        looper->mixAllLayers(samples, samplesToProcess); // mix all layers, no excluded layers
        break;
    default:
        qCritical() << looper->mode << " not implemented yet!";
    }
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

RecordingState::RecordingState(Looper *looper)
    : LooperState(looper)
{

}

void RecordingState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)

    if (looper->mode != Looper::SELECTED_LAYER) {
        int nextLayer = looper->getNextUnlockedLayerIndex();
        looper->setCurrentLayer(nextLayer);
        if (nextLayer == 0 || looper->layerIsLocked(nextLayer))  // stop recording (and start playing) when backing to first layer
            looper->play();
        else
            looper->layers[nextLayer]->zero(); // zero current layer if keep recording
    }
    else { // SELECTED_LAYER_ONLY when recording
        if (!looper->isOverdubbing())  // if not overdubbing we need auto stop recording
            looper->play();
    }
}

void RecordingState::process(SamplesBuffer &samples, uint samplesToProcess)
{
    if (looper->currentLayerIsLocked()) // avoid recording in locked layers
        return;


    if (looper->mode != Looper::SELECTED_LAYER || !looper->isOverdubbing())
        looper->appendInCurrentLayer(samples, samplesToProcess);
    else
        looper->overdubInCurrentLayer(samples, samplesToProcess);

    bool hearingLayersWhileRecording = looper->isHearingLayersWhileRecording() || looper->isOverdubbing();
    if (hearingLayersWhileRecording) {
        const quint8 excludedLayer = looper->mode == Looper::SELECTED_LAYER ? -1 : looper->currentLayerIndex;
        looper->mixAllLayers(samples, samplesToProcess, excludedLayer); // user can hear other layers while recording
    }
}

// -----------------------------------------------------------------------------

WaitingState::WaitingState(Looper *looper)
    : LooperState(looper)
{

}

void WaitingState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)

    looper->startRecording();
}

void WaitingState::process(SamplesBuffer &samples, uint samplesToProcess)
{
    Q_UNUSED(samples)
    Q_UNUSED(samplesToProcess)
}

