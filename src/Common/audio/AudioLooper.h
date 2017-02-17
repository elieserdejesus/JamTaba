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
    void startNewCycle(uint samplesInCycle);

    enum PlayMode
    {
        SEQUENCE, // one layer in each interval
        ALL_LAYERS, // mix and play all layers
        SELECTED_LAYER_ONLY, // ONE selected layer
        RANDOM_LAYERS
    };

    void setPlayMode(PlayMode playMode);

    const std::vector<float> getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const;

    quint8 getCurrentLayerIndex() const;

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    void setMaxLayers(quint8 maxLayers);
    quint8 getMaxLayers() const;

    static const quint8 MAX_LOOP_LAYERS = 4;

private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    class Layer; // internal class
    Layer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;
    quint8 maxLayers;

    enum LooperState
    {
        STOPPED,
        WAITING,
        RECORDING,
        PLAYING
    };

    LooperState state;

    PlayMode playMode;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void playAllLayers(SamplesBuffer &samples, uint samplesToMix);
};

inline void Looper::setPlayMode(PlayMode playMode)
{
    this->playMode = playMode;
}

inline quint8 Looper::getMaxLayers() const
{
    return maxLayers;
}

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
