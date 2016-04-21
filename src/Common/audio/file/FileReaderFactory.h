#ifndef AUDIOFILEREADERFACTORY_H
#define AUDIOFILEREADERFACTORY_H

#include <memory>
#include <QString>

namespace Audio {

class FileReader;

class FileReaderFactory
{
public:
    static std::unique_ptr<Audio::FileReader> createFileReader(const QString &filePath);
};

}//namespace

#endif // AUDIOFILEREADER_H
