#ifndef _LOOPER_PERSISTENCE_H_
#define _LOOPER_PERSISTENCE_H_

#include <QString>

namespace Audio {

class Looper;
class SamplesBuffer;

class LoopSaver
{
public:
    LoopSaver(const QString &savePath, Looper *looper);
    void save(const QString &loopFileName, uint bpm, uint bpi, bool encodeInOggVorbis, uint sampleRate);
private:
    QString savePath;
    Looper *looper;

    void saveSamplesToDisk(const QString &loopFileName, const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, uint sampleRate);

    void saveJsonFile(const QString &loopFileName);

};


struct LoopInfo
{
    quint8 layers;
    quint32 bpm;
    quint16 bpi;
    QString name;
    bool audioIsEncoded;

    LoopInfo(quint8 layers, quint32 bpm, quint16 bpi, const QString &name, bool audioIsEncoded)
        : layers(layers),
          bpm(bpm),
          bpi(bpi),
          name(name),
          audioIsEncoded(audioIsEncoded)
    {
        //
    }

    LoopInfo()
        : layers(0),
          bpm(0),
          bpi(0),
          name(QString()),
          audioIsEncoded(false)
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
    void load(LoopInfo loopInfo, Looper *looper);

    static LoopInfo loadLoopInfo(const QString &loopFilePath);
    static QList<LoopInfo> loadAllLoopsInfo(const QString &loadPath);

private:
    QString loadPath;

    static void loadLayerContent(Looper *looper, const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded);
    static SamplesBuffer loadLoopLayerSamples(const QString &loadPath, const QString &loopName, quint8 layerIndex, bool audioIsEncoded);
};

} // namespace

#endif
