#ifndef OGGFILEREADER_H
#define OGGFILEREADER_H

#include "FileReader.h"

namespace audio {

class OggFileReader : public FileReader
{

public:
    bool read(const QString &filePath, audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override;

};

} // namespace

#endif // OGGFILEREADER_H
