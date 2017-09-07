#ifndef BASESCANNER_H
#define BASESCANNER_H

#include <QStringList>
#include <QProcess>
#include <QFileInfo>

#include "audio/core/PluginDescriptor.h"

class BaseScanner
{

public:
    BaseScanner();
    void start(int argc, char *argv[]);
    virtual ~BaseScanner();

protected:

    QProcess process;
    void writeToProcessOutput(const QString &);

    virtual void scan() = 0;

    virtual void initialize(int argc, char *argv[]) = 0;

};

#endif // BASESCANNER_H
