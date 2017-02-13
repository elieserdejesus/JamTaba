#include "AudioLooper.h"
#include <QDebug>

using namespace Audio;

Looper::Looper()
    : playingBufferedSamples(false),
      currentLayerIndex(0)
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        samplesBufferLayers[l] = new SamplesBuffer(2);
    }
}

Looper::~Looper()
{
    for (int l = 0; l < MAX_LOOP_LAYERS; ++l) {
        delete samplesBufferLayers[l];
    }
}

void Looper::startNewCycle()
{
    qDebug() << "starting new cycle " << samplesBufferLayers[currentLayerIndex]->getFrameLenght() << " samples buffered in previous interval";

    currentLayerIndex = (currentLayerIndex + 1) % MAX_LOOP_LAYERS; // pointing to next layer

    samplesBufferLayers[currentLayerIndex]->zero(); // clear the new current layer before start buffering loop samples

    qDebug() << "new cycle current layer:" << currentLayerIndex;
}

void Looper::playBufferedSamples(bool playBufferedSamples) {
    this->playingBufferedSamples = playBufferedSamples;
}

void Looper::process(SamplesBuffer &samples)
{
    // store/rec current samples
    samplesBufferLayers[currentLayerIndex]->append(samples);

    emit bufferedSamplesPeakAvailable(samples.computePeak().getMaxPeak(), currentLayerIndex);

    //qDebug() << "\tAppending in " << currentLayerIndex << ": " << samples.get(0, 0) << samples.get(0, 1) << samples.get(0, 2);

    if (!playingBufferedSamples) {
        return;
    }

    // render samples from previous interval
    quint8 previousLayer = getPreviousLayerIndex();
    SamplesBuffer *loopBuffer = samplesBufferLayers[previousLayer];

    int samplesToProcess = qMin(samples.getFrameLenght(), loopBuffer->getFrameLenght());

    if (samplesToProcess) {
        //qDebug() << "\tAdding from loop buffer " << loopBuffer->get(0, 0) << loopBuffer->get(0, 1) << loopBuffer->get(0, 2);
        samples.add(*loopBuffer); // mix buffered samples
        loopBuffer->discardFirstSamples(samplesToProcess);
        //qDebug() << "\tResultado" << samples.get(0, 0) << samples.get(0, 1) << samples.get(0, 2);
    }
//    else {
//        qDebug() << "no buffered samples";
//    }
}

quint8 Looper::getPreviousLayerIndex() const
{
    if (currentLayerIndex > 0)
        return currentLayerIndex - 1;

    return MAX_LOOP_LAYERS - 1; // current layer is ZERO, returning the last layer index
}
