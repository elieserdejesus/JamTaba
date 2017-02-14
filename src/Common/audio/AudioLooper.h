#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "core/SamplesBuffer.h"
#include <QObject>

namespace Audio {

class Looper : public QObject
{
    Q_OBJECT

public:
    Looper();
    ~Looper();
    void playBufferedSamples(bool playBufferedSamples);
    void process(SamplesBuffer &samples);
    void startNewCycle(uint samplesInCycle); // create a new layer

    static const quint8 MAX_LOOP_LAYERS = 2;

signals:
    void bufferedSamplesPeakAvailable(float peak, uint samplesCount, quint8 layerIndex);

private:
    bool playingBufferedSamples;

    class Layer;
    Layer* layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;

    quint8 getPreviousLayerIndex() const;

};

} // namespace

#endif
