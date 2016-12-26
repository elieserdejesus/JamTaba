#ifndef VSTPLUGINSCANNER_H
#define VSTPLUGINSCANNER_H

#include <QObject>
#include <QFileInfo>
#include <QProcess>
#include "audio/core/PluginDescriptor.h"
#include "vst/VstHost.h"

class VstPluginScanner : public QObject
{
    Q_OBJECT
public:
    VstPluginScanner(Vst::VstHost *host);
    void start(int argc, char *argv[]);
private:
    QStringList foldersToScan;
    QStringList skipList; //contain blackListed and cached plugins

    void initialize(int argc, char *argv[]);
    void scan();

    Audio::PluginDescriptor getPluginDescriptor(const QFileInfo &pluginFile);

    QProcess process;
    void writeToProcessOutput(const QString &);

    Vst::VstHost *host;
};

#endif // VSTPLUGINSCANNER_H
