#ifndef OGGFILEREADER_H
#define OGGFILEREADER_H

#include "FileReader.h"

namespace Audio {

class OggFileReader : public FileReader
{

public:
    bool read(const QString &filePath, Audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override;

};

} // namespace

#endif // OGGFILEREADER_H
