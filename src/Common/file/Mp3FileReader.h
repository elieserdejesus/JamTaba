#ifndef MP3FILEREADER_H
#define MP3FILEREADER_H

#include "FileReader.h"

namespace Audio {

class Mp3FileReader : public FileReader
{
public:
    void read(const QString &filePath, Audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override;

};

}//namespace

#endif // MP3FILEREADER_H
