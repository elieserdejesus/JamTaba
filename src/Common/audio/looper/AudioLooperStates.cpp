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

void LooperState::process(SamplesBuffer &samples, uint samplesToProcess)
{
    //this code is shared by Playing and Waiting states

    switch (looper->mode) {
    case Looper::SEQUENCE:
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
    if (looper->mode == Looper::Mode::SEQUENCE) {
        looper->incrementCurrentLayer();
    }

    // ALL_LAYERS and SELECTED_LAYER_ONLY play states are not touching in currentLayerIndex
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


    const bool willMixAllLayers = looper->isHearingLayersWhileRecording();
    if (willMixAllLayers) {
        quint8 excludedLayer = looper->currentLayerIndex;

        if (looper->isOverdubbing())
            excludedLayer = -1;

        looper->mixAllLayers(samples, samplesToProcess, excludedLayer); // user can hear other layers while recording
    }
    else if (looper->isOverdubbing()) { // overdubbing only
        looper->mixLayer(looper->currentLayerIndex, samples, samplesToProcess, true); // mix replacing
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
