#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "audio/core/SamplesBuffer.h"
#include "LooperLayer.h"

#include <QtGlobal>
#include <QObject>
#include <QDebug>
#include <QSharedPointer>
#include <QMap>
#include <QMutex>

namespace Audio {

class LooperState;
class PlayingState;
class RecordingState;
class WaitingState;

// +++++++++++++++++++++++++++++=

class Looper : public QObject
{
    Q_OBJECT

    friend class LooperState;
    friend class PlayingState;
    friend class RecordingState;
    friend class WaitingState;

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

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    bool hasLockedLayers() const;

    bool canSave() const;

    void setLayers(quint8 maxLayers);
    quint8 getLayers() const;

    bool canRecord() const;

    void nextMuteState(quint8 layer);

    static const quint8 MAX_LOOP_LAYERS = 8;

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

public slots:
    void reset(); // clear all

signals:
    void stateChanged();
    void modeChanged();
    void maxLayersChanged(quint8 newMaxLayers);
    void currentLayerChanged(quint8 currentLayer);
    void layerChanged(quint8 layer); // layer pan, gain, locked on content changed
    void layerMuteStateChanged(quint8 layer, quint8 state);

private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    bool changed; // used to decide if we can save or not layers content

    LooperLayer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex; // current played layer
    int focusedLayerIndex; // layer clicked by user, used to choose recordint layer. Sometimes focused layer will be equal to currentLayerIndex.
    quint8 maxLayers;

    float mainGain;

    bool resetRequested;
    quint8 newMaxLayersRequested;
    void processChangeRequests();

    void setCurrentLayer(quint8 newLayer);

    AudioPeak lastPeak;

    QSharedPointer<LooperState> state;

    Mode mode;

    struct Options
    {
        QMap<RecordingOption, bool> recordingOptions;
        QMap<PlayingOption, bool> playingOptions;
    };

    QMap<Mode, Options> modeOptions;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void mixAllLayers(SamplesBuffer &samples, uint samplesToMix);
    void mixLockedLayers(SamplesBuffer &samples, uint samplesToMix);

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

    void initialize();

    static QMap<Looper::PlayingOption, bool> getDefaultSupportedPlayingOptions(Looper::Mode mode);
    static QMap<Looper::RecordingOption, bool> getDefaultSupportedRecordingOptions(Looper::Mode mode);

};

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
Q_DECLARE_METATYPE(Audio::Looper::Mode)
Q_DECLARE_METATYPE(Audio::LooperLayer::MuteState)

#endif
