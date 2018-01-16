#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include "audio/core/SamplesBuffer.h"
#include <QString>

namespace audio {

class FileReader
{

public:
    virtual bool read(const QString &filePath, SamplesBuffer& outBuffer, quint32 &sampleRate) = 0;
    virtual ~FileReader(){}
};

} // namespace

#endif // AUDIOFILEREADER_H
