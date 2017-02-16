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
    void setActivated(bool setActivated);
    void process(SamplesBuffer &samples, const AudioPeak &peak);
    void startNewCycle(uint samplesInCycle); // create a new layer

    quint8 getCurrentLayerIndex() const;

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    static const quint8 MAX_LOOP_LAYERS = 4;

signals:
    void samplesPeakAvailable(float peak, uint samplesCount, quint8 layerIndex);

private:

    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    class Layer;
    Layer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;

    Layer *getPreviousLayer() const;

    enum LoopState
    {
        STOPPED,
        WAITING,
        RECORDING,
        PLAYING
    };

    LoopState state;
};

inline bool Looper::isWaiting() const
{
    return state == LoopState::WAITING;
}

inline bool Looper::isPlaying() const
{
    return state == LoopState::PLAYING;
}

inline bool Looper::isRecording() const
{
    return state == LoopState::RECORDING;
}

inline bool Looper::isStopped() const
{
    return state == LoopState::STOPPED;
}

inline quint8 Looper::getCurrentLayerIndex() const
{
    return currentLayerIndex;
}

} // namespace

#endif
