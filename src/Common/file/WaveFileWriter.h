#ifndef WAVEFILEWHITER_H
#define WAVEFILEWHITER_H

#include "FileReader.h"

namespace Audio {

class WaveFileWriter
{
public:
    void write(const QString &filePath, const SamplesBuffer &buffer, quint32 sampleRate);

};

}//namespace

#endif // WAVEFILEWHITER_H
