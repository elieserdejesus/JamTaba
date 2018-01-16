#ifndef _LOOPER_PERSISTENCE_H_
#define _LOOPER_PERSISTENCE_H_

#include <QString>
#include <QSet>
#include <QList>

namespace audio {

class Looper;
class SamplesBuffer;

class LoopSaver
{

public:

    LoopSaver(const QString &savePath, Looper *looper);
    void save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate, quint8 bitDepth);

private:
    QString savePath;
    Looper *looper;

    static QList<quint8> getLockedLayers(Looper *looper);
    static void saveSamplesToDisk(const QString &savePath, const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate, quint8 bitDepth);

    void saveJsonFile(const QString &loopFileName);

};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++=

struct LoopLayerInfo
{
    float pan;
    float gain;
    bool locked;
};

class LoopInfo
{
public:
    LoopInfo(quint32 bpm, quint16 bpi, const QString &name, bool usingEncodedAudio, quint8 mode);
    LoopInfo();

    void addLayer(bool isLocked, float gain, float pan);

    bool isValid() const;

    QString toString(bool showBpm = false) const;

    quint8 getLayersCount() const;

    bool audioIsEncoded() const;

    QString getName() const;

    QList<LoopLayerInfo> getLayersInfo() const;

    quint8 getLooperMode() const;

    quint16 getBpi() const;
    quint32 getBpm() const;

private:
    quint32 bpm;
    quint16 bpi;
    QString name;
    bool usingEncodedAudio;
    QList<LoopLayerInfo> layers;
    quint8 looperMode;
};

inline quint16 LoopInfo::getBpi() const
{
    return bpi;
}

inline quint32 LoopInfo::getBpm() const
{
    return bpm;
}

inline quint8 LoopInfo::getLooperMode() const
{
    return looperMode;
}

inline quint8 LoopInfo::getLayersCount() const
{
    return layers.size();
}

inline bool LoopInfo::audioIsEncoded() const
{
    return usingEncodedAudio;
}

inline QString LoopInfo::getName() const
{
    return name;
}

inline QList<LoopLayerInfo> LoopInfo::getLayersInfo() const
{
    return layers;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++=

class LoopLoader
{

public:
    explicit LoopLoader(const QString &loadPath);
    void load(LoopInfo loopInfo, Looper *looper, uint currentSampleRate, quint32 samplesPerInterval);

    static LoopInfo loadLoopInfo(const QString &loopFilePath);
    static QList<LoopInfo> loadLoopsInfo(const QString &loadPath, quint32 bpmToMatch);
    static bool loadAudioFile(const QString &filePath, uint currentSampleRate, SamplesBuffer &out);

    static bool loadLoopLayerSamples(const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded, uint currentSampleRate, SamplesBuffer &out);

private:
    QString loadPath;

};

} // namespace

#endif
