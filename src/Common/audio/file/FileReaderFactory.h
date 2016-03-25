#ifndef AUDIOFILEREADERFACTORY_H
#define AUDIOFILEREADERFACTORY_H

#include <memory>

namespace Audio {

class FileReader;

class FileReaderFactory
{
public:
    static std::unique_ptr<Audio::FileReader> createFileReader(const QString &filePath);
};

}//namespace

#endif // AUDIOFILEREADER_H
