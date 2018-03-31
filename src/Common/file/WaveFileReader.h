#ifndef WAVEFILEREADER_H
#define WAVEFILEREADER_H

#include "FileReader.h"

namespace audio {

class WaveFileReader : public FileReader
{

public:
    bool read(const QString &filePath, audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override;

};

} // namespace

#endif // AUDIOFILEREADER_H
