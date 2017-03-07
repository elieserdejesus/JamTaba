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
    void save(bool encodeInOggVorbis, uint loopLenght, uint sampleRate);
private:
    QString savePath;
    Looper *looper;

    void saveSamplesToDisk(const SamplesBuffer &buffer, quint8 layerIndex, bool encodeInOggVorbis, uint sampleRate);
};

} // namespace

#endif
