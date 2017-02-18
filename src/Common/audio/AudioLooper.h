#ifndef _AUDIO_LOOPER_
#define _AUDIO_LOOPER_

#include "core/SamplesBuffer.h"

#include <QtGlobal>
#include <QObject>

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
    void stop();

    enum PlayMode
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

    static QString getPlayModeString(PlayMode playMode);

    void setPlayMode(PlayMode playMode);
    PlayMode getPlayMode() const;

    const std::vector<float> getLayerPeaks(quint8 layerIndex, uint samplesPerPeak) const;

    quint8 getCurrentLayerIndex() const;

    bool isWaiting() const;
    bool isPlaying() const;
    bool isRecording() const;
    bool isStopped() const;

    void setMaxLayers(quint8 maxLayers);
    quint8 getMaxLayers() const;

    void setState(LooperState state);

    bool canPlay(uint intervalLenght) const; // all layers have available samples?

    static const quint8 MAX_LOOP_LAYERS = 4;

signals:
    void stateChanged();
    void maxLayersChanged(quint8 newMaxLayers);
    void currentLayerChanged(quint8 currentLayer);
private:
    uint intervalLenght; // in samples
    uint intervalPosition; // in samples

    class Layer; // internal class
    Layer *layers[MAX_LOOP_LAYERS];
    quint8 currentLayerIndex;
    quint8 maxLayers;

    void setCurrentLayer(quint8 newLayer);

    LooperState state;

    PlayMode playMode;

    void mixLayer(quint8 layerIndex, SamplesBuffer &samples, uint samplesToMix);
    void playAllLayers(SamplesBuffer &samples, uint samplesToMix);
};

inline void Looper::setPlayMode(PlayMode playMode)
{
    this->playMode = playMode;
    qDebug() << "playMode setted to:" << playMode;
}

inline Looper::PlayMode Looper::getPlayMode() const
{
    return playMode;
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

Q_DECLARE_METATYPE(Audio::Looper::PlayMode)

} // namespace

#endif
