#include "LooperStates.h"
#include "Looper.h"
#include "LooperLayer.h"

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

void StoppedState::mixTo(SamplesBuffer &samples, uint samplesToProcess)
{
    Q_UNUSED(samples)
    Q_UNUSED(samplesToProcess)
}

void StoppedState::addBuffer(const SamplesBuffer &samples, uint samplesToProcess)
{
    Q_UNUSED (samples)
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
    if (looper->mode == Looper::Mode::Sequence) {
        looper->incrementCurrentLayer();
    }

    // ALL_LAYERS and SELECTED_LAYER_ONLY play states are not touching in currentLayerIndex
}

void PlayingState::addBuffer(const SamplesBuffer &samples, uint samplesToProcess)
{
    Q_UNUSED(samples);
    Q_UNUSED(samplesToProcess);
}

void PlayingState::mixTo(SamplesBuffer &samples, uint samplesToProcess)
{
    switch (looper->mode) {
    case Looper::Sequence:
    case Looper::SelectedLayer:
        looper->mixCurrentLayerTo(samples, samplesToProcess);
        break;
    case Looper::AllLayers:
    {
        bool isPlayingLockedLayersOnly = looper->getOption(Looper::PlayLockedLayers);
        if (!isPlayingLockedLayersOnly)
            looper->mixAllLayers(samples, samplesToProcess); // mix all layers, no excluded layers
        else
            looper->mixLockedLayers(samples, samplesToProcess); // mix locked only
        break;
    }
    default:
        qCritical() << looper->mode << " not implemented yet!";
    }
}

// -------------------------------------------------------

RecordingState::RecordingState(Looper *looper, quint8 recordingLayer)
    : LooperState(looper),
      firstRecordingLayer(recordingLayer)
{

}

void RecordingState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)

    const bool isOverdubbing = looper->getOption(Looper::Overdub);
    const bool isPlayingLockedLayersOnly = looper->getOption(Looper::PlayLockedLayers);

    if (looper->mode != Looper::SelectedLayer) {
        if (!isOverdubbing) {
            int previousLayer = looper->getCurrentLayerIndex();
            int nextLayer = looper->getNextUnlockedLayerIndex();
            looper->setCurrentLayer(nextLayer);
            if ((nextLayer == firstRecordingLayer || nextLayer == 0) || looper->layerIsLocked(nextLayer) || previousLayer == looper->getLayers() - 1) {  // stop recording (and start playing) when backing to first rec layer OR last layer is reached
                if (isPlayingLockedLayersOnly)
                    looper->setCurrentLayer(looper->getFirstLockedLayerIndex());
                looper->play();
            }
            else {
                looper->layers[nextLayer]->zero(); // zero current layer if keep recording
            }
        }
    }
    else { // SELECTED_LAYER_ONLY when recording
        if (!isOverdubbing)  // if not overdubbing we need auto stop recording
            looper->play();
    }
}

void RecordingState::mixTo(SamplesBuffer &samples, uint samplesToProcess)
{
    samples.zero(); // zeroing samples before mix to avoid re-add buffered audio and double the incomming samples

    const bool hearingAllLayers = looper->getMode() == Looper::AllLayers || looper->getOption(Looper::HearAllLayers);
    if (hearingAllLayers) {
        if (looper->getOption(Looper::PlayLockedLayers) && looper->hasLockedLayers()) {
            looper->mixLockedLayers(samples, samplesToProcess);
            looper->mixLayer(looper->currentLayerIndex, samples, samplesToProcess);
        }
        else {
            looper->mixAllLayers(samples, samplesToProcess); // user can hear other layers while recording
        }
    }
    else {
        looper->mixLayer(looper->currentLayerIndex, samples, samplesToProcess);
    }
}

void RecordingState::addBuffer(const SamplesBuffer &samples, uint samplesToProcess)
{
    if (looper->currentLayerIsLocked()) // avoid recording in locked layers
        return;

    bool isOverdubbing = looper->getOption(Looper::Overdub);
    if (!isOverdubbing)
        looper->appendInCurrentLayer(samples, samplesToProcess);
    else
        looper->overdubInCurrentLayer(samples, samplesToProcess);
}

// -----------------------------------------------------------------------------

WaitingToRecordState::WaitingToRecordState(Looper *looper)
    : LooperState(looper),
      lastInputBuffer(2)
{

}

void WaitingToRecordState::mixTo(SamplesBuffer &samples, uint samplesToProcess)
{
    samples.zero();

    looper->processBufferUsingCurrentLayerSettings(lastInputBuffer); // apply current layer gain and pan settings
    samples.add(lastInputBuffer); // mix the current incomming/input buffer

    const bool hearingAllLayers = looper->getMode() == Looper::AllLayers || looper->getOption(Looper::HearAllLayers);
    if (hearingAllLayers) {
        if (looper->getOption(Looper::PlayLockedLayers))
            looper->mixLockedLayers(samples, samplesToProcess);
        else
            looper->mixAllLayers(samples, samplesToProcess); // user can hear other layers while recording
    }
    else {
        looper->mixLayer(looper->currentLayerIndex, samples, samplesToProcess);
    }
}

void WaitingToRecordState::addBuffer(const SamplesBuffer &samples, uint samplesToProcess)
{
    // not buffering samples when waiting to record. Incomming samples are stored in a secondary buffer
    lastInputBuffer.setFrameLenght(samplesToProcess);
    lastInputBuffer.set(samples);
}

void WaitingToRecordState::handleNewCycle(uint samplesInCycle)
{
    Q_UNUSED(samplesInCycle)

    looper->startRecording();
}
