#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "core/SamplesBuffer.h"

#include <QtGlobal>

namespace Audio {

class Looper
{

public:
    Looper();
    ~Looper();
    void setActivated(bool setActivated);
    void process(SamplesBuffer &samples);
    void startNewCycle(uint samplesInCycle); // create a new layer

    const std::vector<float> getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const;

    quint8 getCurrentLayerIndex() const;

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    static const quint8 MAX_LOOP_LAYERS = 4;

private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    class Layer;
    Layer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;

    Layer *getPreviousLayer() const;

    enum LooperState
    {
        STOPPED,
        WAITING,
        RECORDING,
        PLAYING
    };

    LooperState state;
};

inline bool Looper::isWaiting() const
{
    return state == LooperState::WAITING;
}

inline bool Looper::isPlaying() const
{
    return state == LooperState::PLAYING;
}

inline bool Looper::isRecording() const
{
    return state == LooperState::RECORDING;
}

inline bool Looper::isStopped() const
{
    return state == LooperState::STOPPED;
}

inline quint8 Looper::getCurrentLayerIndex() const
{
    return currentLayerIndex;
}

} // namespace

#endif
