#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "audio/core/SamplesBuffer.h"
#include "LooperLayer.h"
#include "LooperPersistence.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QMap>
#include <QMutex>

#define MAX_LOOP_LAYERS 8

namespace audio {

class LooperState;
class PlayingState;
class RecordingState;
class WaitingToRecordState;

// +++++++++++++++++++++++++++++=

class Looper : public QObject
{
    Q_OBJECT

    friend class LooperState;
    friend class PlayingState;
    friend class RecordingState;
    friend class WaitingToRecordState;

public:

    enum Mode
    {
        Sequence, // one layer in each interval
        AllLayers, // mix and play all layers
        SelectedLayer
    };

    Looper();
    Looper(Looper::Mode initialMode, quint8 maxLayers);
    ~Looper();
    void addBuffer(const SamplesBuffer &samples); // recording
    void mixToBuffer(SamplesBuffer &samples); // playing/mixing

    void setMainGain(float gain);
    float getMainGain() const;

    AudioPeak getLastPeak() const;

    void setLayerSamples(quint8 layer, const SamplesBuffer &samples);

    void startNewCycle(uint samplesInCycle);

    void selectLayer(quint8 layerIndex);
    bool canSelectLayers() const;

    void clearCurrentLayer();
    void clearLayer(quint8 layer);
    bool canClearLayer(quint8 layer) const;
    bool canLockLayer(quint8 layer) const;

    void setLayerLockedState(quint8 layerIndex, bool locked);
    void toggleLayerLockedState(quint8 layerIndex);
    bool layerIsLocked(quint8 layerIndex) const;

    bool layerIsValid(quint8 layerIndex) const;
    int getLastValidLayer() const;
    bool isEmpty() const; // all layers are empty
    bool isFull() const; // no empty layers

    void setLayerGain(quint8 layerIndex, float gain);
    void setLayerPan(quint8 layerIndex, float pan);

    void stop();
    void play();

    float getLayerGain(quint8 layer) const;
    float getLayerPan(quint8 layer) const;

    QList<SamplesBuffer> getLayersSamples() const;

    static QString getModeString(Mode mode);

    void setMode(Mode mode);
    Mode getMode() const;

    const std::vector<float> getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const;

    quint8 getCurrentLayerIndex() const;
    quint8 getFocusedLayerIndex() const;

    bool isWaitingToRecord() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    bool hasLockedLayers() const;

    bool canSave() const;

    bool isChanged() const;

    void setLayers(quint8 maxLayers, bool processChangeRequestNow = false);
    quint8 getLayers() const;

    bool canRecord() const;

    void nextMuteState(quint8 layer);

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
        PlayLockedLayers,
        PlayNonEmptyLayers
    };

    void setOption(RecordingOption option, bool enabled);
    bool getOption(RecordingOption option) const;

    void setOption(PlayingOption option, bool enabled);
    bool getOption(PlayingOption option) const;
    bool optionIsSupportedInCurrentMode(PlayingOption option) const;
    bool optionIsSupportedInCurrentMode(RecordingOption option) const;

    uint getIntervalLenght() const;

    void setChanged(bool changed);
    void setLoading(bool loading);

    bool isWaitingToStopInNextInterval() const;
    void waitToStopInNextInterval();

    void setLoopName(const QString loopName);
    QString getLoopName() const;

    void setActivated(bool activated);

public slots:
    void resetLayersContent(); // clear all

signals:
    void stateChanged();
    void modeChanged();
    void maxLayersChanged(quint8 newMaxLayers);
    void currentLayerChanged(quint8 currentLayer);
    void layerChanged(quint8 layer); // layer pan, gain, locked or content changed
    void layerMuteStateChanged(quint8 layer, quint8 state);
    void layersContentErased();
    void currentLoopNameChanged(const QString &loopName);

private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    bool changed; // used to decide if we can save or not layers content
    bool loading;
    bool waitingToStop; // waiting to stop in next interval

    bool activated;

    LooperLayer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex; // current played layer
    int focusedLayerIndex; // layer clicked by user, used to choose recording layer. Sometimes focused layer will be equal to currentLayerIndex.
    quint8 maxLayers;

    float mainGain;

    bool resetRequested;
    quint8 newMaxLayersRequested;
    void processChangeRequests();

    void setCurrentLayer(quint8 newLayer);

    AudioPeak lastPeak;

    QSharedPointer<LooperState> state;

    QString loopName; // can be empty if no loop is loaded

    Mode mode;

    struct Options
    {
        QMap<RecordingOption, bool> recordingOptions;
        QMap<PlayingOption, bool> playingOptions;
    };

    Options modeOptions[3]; // 3 modes

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void mixAllLayers(SamplesBuffer &samples, uint samplesToMix);
    void mixLockedLayers(SamplesBuffer &samples, uint samplesToMix);

    void setState(LooperState *state);

    int getFirstLockedLayerIndex(quint8 startingFrom = 0) const;
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

    void processBufferUsingCurrentLayerSettings(SamplesBuffer &buffer);

    static QMap<Looper::PlayingOption, bool> getDefaultSupportedPlayingOptions(Looper::Mode mode);
    static QMap<Looper::RecordingOption, bool> getDefaultSupportedRecordingOptions(Looper::Mode mode);

};

inline float Looper::getMainGain() const
{
    return mainGain;
}

inline QString Looper::getLoopName() const
{
    return loopName;
}

inline void Looper::setLoading(bool loading)
{
    this->loading = loading;
}

inline bool Looper::isWaitingToStopInNextInterval() const
{
    return waitingToStop;
}

inline bool Looper::isChanged() const
{
    return changed;
}

inline uint Looper::getIntervalLenght() const
{
    return intervalLenght;
}

inline bool Looper::optionIsSupportedInCurrentMode(PlayingOption option) const
{
    return modeOptions[mode].playingOptions.keys().contains(option);
}

inline bool Looper::optionIsSupportedInCurrentMode(RecordingOption option) const
{
    return modeOptions[mode].recordingOptions.keys().contains(option);
}

inline void Looper::setOption(Looper::RecordingOption option, bool value)
{
    if (!optionIsSupportedInCurrentMode(option)) {
        return;
    }

    modeOptions[mode].recordingOptions[option] = value;
}

inline void Looper::setOption(Looper::PlayingOption option, bool value)
{
    if (!optionIsSupportedInCurrentMode(option)) {
        return;
    }

    modeOptions[mode].playingOptions[option] = value;
}

inline bool Looper::getOption(Looper::PlayingOption option) const
{
    if (modeOptions[mode].playingOptions.contains(option))
        return modeOptions[mode].playingOptions[option];

    return false;
}

inline bool Looper::getOption(Looper::RecordingOption option) const
{
    if (modeOptions[mode].recordingOptions.contains(option))
        return modeOptions[mode].recordingOptions[option];

    return false;
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

inline quint8 Looper::getFocusedLayerIndex() const
{
    return focusedLayerIndex;
}

} // namespace


// declaring structs to use in signal/slots
Q_DECLARE_METATYPE(audio::Looper::Mode)
Q_DECLARE_METATYPE(audio::LooperLayer::MuteState)

#endif
