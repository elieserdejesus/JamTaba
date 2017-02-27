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

    friend class LooperState;
    friend class PlayingState;
    friend class RecordingState;
    friend class WaitingState;

public:
    Looper();
    ~Looper();
    void process(SamplesBuffer &samples);
    void startNewCycle(uint samplesInCycle);

    void selectLayer(quint8 layerIndex);
    bool canSelectLayers() const;

    void clearAllLayers();
    void clearCurrentLayer();
    void clearLayer(quint8 layer);
    bool canClearLayer(quint8 layer) const;
    bool canLockLayer(quint8 layer) const;

    void setLayerLockedState(quint8 layerIndex, bool locked);
    void toggleLayerLockedState(quint8 layerIndex);
    bool layerIsLocked(quint8 layerIndex) const;

    bool layerIsValid(quint8 layerIndex) const;

    bool currentModeHasRecordingProperties() const;
    bool currentModeHasPlayingProperties() const;

    void stop();
    void play();

    enum Mode
    {
        SEQUENCE, // one layer in each interval
        ALL_LAYERS, // mix and play all layers
        SELECTED_LAYER
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

    bool isRandomizing() const;
    void setRandomizing(bool randomizingLayers);

    bool isPlayingLockedLayersOnly() const;
    void setPlayingLockedLayersOnly(bool playingLocked);

    bool isOverdubbing() const;
    void setOverdubbing(bool overdubbing);

    bool hasLockedLayers() const;

    void setLayers(quint8 maxLayers);
    quint8 getLayers() const;

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

    // playing attributes
    bool randomizing;
    bool playingLockedLayersOnly;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix, bool replacing);
    void mixAllLayers(SamplesBuffer &samples, uint samplesToMix, int exceptLayer = -1);

    void setState(LooperState *state);

    int getFirstUnlockedLayerIndex(quint8 startingFrom = 0) const;
    int getNextUnlockedLayerIndex() const;

    void appendInCurrentLayer(const SamplesBuffer &samples, uint samplesToAppend);
    void overdubInCurrentLayer(const SamplesBuffer &samples, uint samplesToMix);
    void mixCurrentLayerTo(SamplesBuffer &samples, uint samplesToMix);

    void startRecording();

    void incrementCurrentLayer();
    void incrementLockedLayer();

    uint getLockedLayers() const;

    bool currentLayerIsLocked() const;

};

inline bool Looper::hasLockedLayers() const
{
    return getLockedLayers() > 0;
}

inline void Looper::setRandomizing(bool randomizingLayers)
{
    this->randomizing = randomizingLayers;
}

inline void Looper::setPlayingLockedLayersOnly(bool playingLocked)
{
    this->playingLockedLayersOnly = playingLocked;
}

inline bool Looper::isRandomizing() const
{
    return mode == Looper::SEQUENCE && randomizing;
}

inline bool Looper::isPlayingLockedLayersOnly() const
{
    return mode == Looper::SEQUENCE && playingLockedLayersOnly && hasLockedLayers();
}

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
    return hearingOtherLayers && maxLayers > 1 && (mode == Looper::ALL_LAYERS || mode == Looper::SELECTED_LAYER);
}

inline Looper::Mode Looper::getMode() const
{
    return mode;
}

inline quint8 Looper::getLayers() const
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
