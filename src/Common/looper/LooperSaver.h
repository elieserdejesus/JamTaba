#ifndef _LOOPER_SAVER_H_
#define _LOOPER_SAVER_H_

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
};

} // namespace

#endif
