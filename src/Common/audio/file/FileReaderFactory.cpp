#include "FileReaderFactory.h"

#include <QFileInfo>
#include "WaveFileReader.h"

using namespace Audio;

class NullFileReader : public FileReader
{
public:
    inline void read(const QString &filePath, Audio::SamplesBuffer &outBuffer, quint32 &sampleRate) override{
        Q_UNUSED(filePath)
        Q_UNUSED(outBuffer)
        Q_UNUSED(sampleRate)
    }
};

std::unique_ptr<FileReader> FileReaderFactory::createFileReader(const QString &filePath)
{
    QString fileSuffix = QFileInfo(filePath).suffix();
    if (fileSuffix == "wav") {
        return std::make_unique<Audio::WaveFileReader>();
    }
    return std::make_unique<NullFileReader>();
}
