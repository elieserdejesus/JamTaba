#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "../core/SamplesBuffer.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QMap>

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

    bool hasLockedLayers() const;

    void setLayers(quint8 maxLayers);
    quint8 getLayers() const;

    bool canRecord() const;

    static const quint8 MAX_LOOP_LAYERS = 4;

    void toggleRecording();
    void togglePlay();

    enum RecordingOption
    {
        Overdub,
        HearAllLayers
    };

    enum PlayingOption
    {
        RandomizeLayers,
        PlayLockedLayers
    };

    QList<RecordingOption> getRecordingOptions() const;
    void setRecordingOption(RecordingOption option, bool enabled);
    bool getRecordingOption(RecordingOption option) const;

    QList<PlayingOption> getPlayingOptions() const;
    void setPlayingOption(PlayingOption option, bool enabled);
    bool getPlayingOption(PlayingOption option) const;

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

    struct Options
    {
        QMap<RecordingOption, bool> recordingOptions;
        QMap<PlayingOption, bool> playingOptions;
    };

    QMap<Mode, Options> modeOptions;

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

    static QMap<Looper::PlayingOption, bool> getDefaultPlayingOptions(Looper::Mode mode);
    static QMap<Looper::RecordingOption, bool> getDefaultRecordingOptions(Looper::Mode mode);

};

inline void Looper::setRecordingOption(Looper::RecordingOption option, bool value)
{
    modeOptions[mode].recordingOptions[option] = value;
}

inline void Looper::setPlayingOption(Looper::PlayingOption option, bool value)
{
    modeOptions[mode].playingOptions[option] = value;
}

inline bool Looper::getPlayingOption(Looper::PlayingOption option) const
{
    if (modeOptions[mode].playingOptions.contains(option))
        return modeOptions[mode].playingOptions[option];

    return false;
}

inline bool Looper::getRecordingOption(Looper::RecordingOption option) const
{
    if (modeOptions[mode].recordingOptions.contains(option))
        return modeOptions[mode].recordingOptions[option];

    return false;
}

inline QList<Looper::RecordingOption> Looper::getRecordingOptions() const
{
    return modeOptions[mode].recordingOptions.keys();
}

inline QList<Looper::PlayingOption> Looper::getPlayingOptions() const
{
    return modeOptions[mode].playingOptions.keys();
}

inline bool Looper::hasLockedLayers() const
{
    return getLockedLayers() > 0;
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
