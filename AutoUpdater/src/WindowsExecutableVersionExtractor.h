#ifndef WINDOWSEXECUTABLEVERSIONEXTRACTOR_H
#define WINDOWSEXECUTABLEVERSIONEXTRACTOR_H

#include "AutoUpdater.h"

class WindowsExecutableVersionExtractor : public ExecutableVersionExtractor
{
public:
    WindowsExecutableVersionExtractor();
    QString getVersionString(QString executablePath);
private:
    QString extract(QString executablePath);

    std::string GetLastErrorAsString();
};

#endif // WINDOWSEXECUTABLEVERSIONEXTRACTOR_H
