#ifndef AUDIOFILEREADER_H
#define AUDIOFILEREADER_H

#include "audio/core/SamplesBuffer.h"
#include <QString>

namespace Audio {

class FileReader
{

public:
    virtual bool read(const QString &filePath, Audio::SamplesBuffer& outBuffer, quint32 &sampleRate) = 0;
    virtual ~FileReader(){}
};

} // namespace

#endif // AUDIOFILEREADER_H
