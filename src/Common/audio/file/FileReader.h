#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include "audio/core/SamplesBuffer.h"

namespace Audio {

class FileReader
{
public:
    virtual void read(const QString &filePath, Audio::SamplesBuffer& outBuffer, quint32 &sampleRate) = 0;
};

}//namespace

#endif // AUDIOFILEREADER_H
