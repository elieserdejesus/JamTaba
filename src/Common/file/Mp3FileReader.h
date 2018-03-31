#ifndef MP3FILEREADER_H
#define MP3FILEREADER_H

#include "FileReader.h"

namespace audio {

class Mp3FileReader : public FileReader
{

public:
    bool read(const QString &filePath, audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override;

};

} // namespace

#endif // MP3FILEREADER_H
