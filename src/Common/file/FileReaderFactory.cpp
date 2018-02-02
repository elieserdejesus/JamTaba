#include "FileReaderFactory.h"

#include <QFileInfo>
#include "WaveFileReader.h"
#include "OggFileReader.h"
#include "Mp3FileReader.h"

using audio::FileReader;
using audio::FileReaderFactory;

class NullFileReader : public FileReader
{

public:
    inline bool read(const QString &filePath, audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override
    {
        Q_UNUSED(filePath)
        Q_UNUSED(outBuffer)
        Q_UNUSED(sampleRate)

        return false;
    }
};

std::unique_ptr<FileReader> FileReaderFactory::createFileReader(const QString &filePath)
{
    QString fileSuffix = QFileInfo(filePath).suffix();
    if (fileSuffix == "wav")
        return std::unique_ptr<audio::WaveFileReader>(new WaveFileReader());
    else if (fileSuffix == "ogg")
        return std::unique_ptr<audio::OggFileReader>(new OggFileReader());
    else if (fileSuffix == "mp3")
        return std::unique_ptr<audio::Mp3FileReader>(new Mp3FileReader());

    return std::unique_ptr<NullFileReader>(new NullFileReader());
}
