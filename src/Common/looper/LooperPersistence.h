#ifndef _LOOPER_PERSISTENCE_H_
#define _LOOPER_PERSISTENCE_H_

#include <QString>
#include <QSet>

namespace Audio {

class Looper;
class SamplesBuffer;

class LoopSaver
{
public:
    LoopSaver(const QString &savePath, Looper *looper);
    void save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate);
private:
    QString savePath;
    Looper *looper;

    static QList<quint8> getLockedLayers(Looper *looper);
    static void saveSamplesToDisk(const QString &savePath, const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, float vorbisQuality, uint sampleRate);

    void saveJsonFile(const QString &loopFileName);

};


struct LoopInfo
{
    quint8 layers;
    quint32 bpm;
    quint16 bpi;
    QString name;
    bool audioIsEncoded;
    QSet<quint8> lockedLayers;

    LoopInfo(quint8 layers, quint32 bpm, quint16 bpi, const QString &name, bool audioIsEncoded)
        : layers(layers),
          bpm(bpm),
          bpi(bpi),
          name(name),
          audioIsEncoded(audioIsEncoded),
          lockedLayers(QSet<quint8>())
    {
        //
    }

    LoopInfo()
        : layers(0),
          bpm(0),
          bpi(0),
          name(QString()),
          audioIsEncoded(false),
          lockedLayers(QSet<quint8>())
    {

    }

    bool isValid() const
    {
        return !name.isEmpty() && bpm > 0 && bpi > 0 && layers > 0;
    }

    QString toString() const
    {
        return name + " (" + QString::number(bpm) + " BPM, " + QString::number(bpi) + " BPI, " + QString::number(layers) + " layers)";
    }
};

class LoopLoader
{
public:
    LoopLoader(const QString &loadPath);
    void load(LoopInfo loopInfo, Looper *looper, uint currentSampleRate);

    static LoopInfo loadLoopInfo(const QString &loopFilePath);
    static QList<LoopInfo> loadAllLoopsInfo(const QString &loadPath);

private:
    QString loadPath;

    static SamplesBuffer loadLoopLayerSamples(const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded, uint currentSampleRate);
};

} // namespace

#endif
