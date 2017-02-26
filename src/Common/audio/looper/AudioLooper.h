#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "../core/SamplesBuffer.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>

namespace Audio {

class LooperState;
class PlayingState;
class RecordingState;
class WaitingState;
class LooperLayer;

// +++++++++++++++++++++++++++++=

class Looper : public QObject
{
    Q_OBJECT

    friend class PlayingState;
    friend class RecordingState;
    friend class WaitingState;

public:
    Looper();
    ~Looper();
    void process(SamplesBuffer &samples);
    void startNewCycle(uint samplesInCycle);

    void selectLayer(quint8 layerIndex);

    void clearAllLayers();
    void clearCurrentLayer();
    void clearLayer(quint8 layer);
    bool canClearLayer(quint8 layer) const;
    bool canLockLayer(quint8 layer) const;

    void setLayerLockedState(quint8 layerIndex, bool locked);
    void toggleLayerLockedState(quint8 layerIndex);
    bool layerIsLocked(quint8 layerIndex) const;

    bool layerIsValid(quint8 layerIndex) const;

    void stop();
    void play();

    enum Mode
    {
        SEQUENCE, // one layer in each interval
        ALL_LAYERS, // mix and play all layers
        SELECTED_LAYER, // ONE selected layer
        RANDOM_LAYERS
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

    bool isHearingLayersWhileRecording() const;
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

    LooperLayer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;
    quint8 maxLayers;

    void setCurrentLayer(quint8 newLayer);

    QSharedPointer<LooperState> state;

    Mode mode;

    // recording attributes
    bool overdubbing;
    bool hearingOtherLayers;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void mixAllLayers(SamplesBuffer &samples, uint samplesToMix, int exceptLayer = -1);

    void setState(LooperState *state);

    int getFirstUnlockedLayerIndex(quint8 startingFrom = 0) const;
    int getNextUnlockedLayerIndex() const;

    void appendInCurrentLayer(const SamplesBuffer &samples, uint samplesToAppend);
    void overdubInCurrentLayer(const SamplesBuffer &samples, uint samplesToMix);
    void mixCurrentLayerTo(SamplesBuffer &samples, uint samplesToMix);

    void startRecording();

    void incrementCurrentLayer();
    void randomizeCurrentLayer();

    bool currentLayerIsLocked() const;

 };

inline void Looper::setOverdubbing(bool overdubbing)
{
    this->overdubbing = overdubbing;
}

inline bool Looper::isOverdubbing() const
{
    return overdubbing && mode == Looper::SELECTED_LAYER;
}

inline bool Looper::isHearingLayersWhileRecording() const
{
    return hearingOtherLayers && mode == Looper::ALL_LAYERS;
}

inline Looper::Mode Looper::getMode() const
{
    return mode;
}

inline quint8 Looper::getMaxLayers() const
{
    return maxLayers;
}

inline quint8 Looper::getCurrentLayerIndex() const
{
    return currentLayerIndex;
}

} // namespace

Q_DECLARE_METATYPE(Audio::Looper::Mode)

#endif
