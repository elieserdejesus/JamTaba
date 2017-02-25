#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "core/SamplesBuffer.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>

namespace Audio {

class Looper : public QObject
{
    Q_OBJECT

public:
    Looper();
    ~Looper();
    void process(SamplesBuffer &samples);
    void startNewCycle(uint samplesInCycle);

    void selectLayer(quint8 layerIndex);

    void clearAllLayers();
    void clearSelectedLayer();

    void setLayerLockedState(quint8 layerIndex, bool locked);
    void toggleLayerLockedState(quint8 layerIndex);
    bool layerIsLocked(quint8 layerIndex) const;

    bool layerIsValid(quint8 layerIndex) const;

    void stop();

    enum Mode
    {
        SEQUENCE, // one layer in each interval
        ALL_LAYERS, // mix and play all layers
        SELECTED_LAYER_ONLY, // ONE selected layer
        RANDOM_LAYERS
    };

    enum LooperState
    {
        STOPPED,
        WAITING,
        RECORDING,
        PLAYING
    };

    static QString getModeString(Mode mode);

    void setMode(Mode mode);
    Mode getMode() const;

    const std::vector<float> getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const;

    quint8 getCurrentLayerIndex() const;

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    bool isHearingOtherLayersWhileRecording() const;
    void setHearingOtherLayersWhileRecording(bool hearingOtherLayers);

    bool isOverdubbing() const;
    void setOverdubbing(bool overdubbing);

    void setMaxLayers(quint8 maxLayers);
    quint8 getMaxLayers() const;

    bool canRecord() const;

    static const quint8 MAX_LOOP_LAYERS = 4;

    void toggleRecording();
    void togglePlay();

signals:
    void stateChanged();
    void modeChanged();
    void maxLayersChanged(quint8 newMaxLayers);
    void currentLayerChanged(quint8 currentLayer);
    void layerLockedStateChanged(quint8 currentLayer, bool locked);
private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    bool hearingOtherLayersWhileRecording;
    bool overdubbing;

    class Layer; // internal class
    Layer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;
    quint8 maxLayers;

    void setCurrentLayer(quint8 newLayer);

    LooperState state;

    Mode mode;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void mixAllLayers(SamplesBuffer &samples, uint samplesToMix, int exceptLayer);

    void setState(LooperState state);

    int getFirstUnlockedLayerIndex(quint8 startingFrom = 0) const;
};

inline bool Looper::isOverdubbing() const
{
    return overdubbing;
}

inline bool Looper::isHearingOtherLayersWhileRecording() const
{
    return hearingOtherLayersWhileRecording;
}

inline Looper::Mode Looper::getMode() const
{
    return mode;
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

Q_DECLARE_METATYPE(Audio::Looper::Mode)

#endif
