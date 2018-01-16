#ifndef WAVEFILEWHITER_H
#define WAVEFILEWHITER_H

#include "FileReader.h"

namespace audio {

class WaveFileWriter
{

public:
    void write(const QString &filePath, const SamplesBuffer &buffer, quint32 sampleRate, quint8 bitDepth);

};

} // namespace

#endif // WAVEFILEWHITER_H
