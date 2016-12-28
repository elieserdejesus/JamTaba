#ifndef AUPLUGINSCANNER_H
#define AUPLUGINSCANNER_H

#include <QObject>
#include <QFileInfo>
#include <QProcess>
#include "audio/core/PluginDescriptor.h"

class AUPluginScanner : public QObject
{
    Q_OBJECT

public:
    AUPluginScanner();
    void start(int argc, char *argv[]);
private:
    QStringList foldersToScan;
    QStringList skipList; //contain blackListed and cached plugins

    void initialize(int argc, char *argv[]);
    void scan();

    Audio::PluginDescriptor getPluginDescriptor(const QFileInfo &pluginFile);

    QProcess process;
    void writeToProcessOutput(const QString &);
};

#endif // AUPLUGINSCANNER_H
