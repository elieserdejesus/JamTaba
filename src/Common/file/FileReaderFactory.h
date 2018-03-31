#ifndef AUDIOFILEREADERFACTORY_H
#define AUDIOFILEREADERFACTORY_H

#include <memory>
#include <QString>

namespace audio {

class FileReader;

class FileReaderFactory
{

public:
    static std::unique_ptr<audio::FileReader> createFileReader(const QString &filePath);
};

} // namespace

#endif // AUDIOFILEREADER_H
