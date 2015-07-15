#pragma once
#include <QFileInfo>
#include <QThread>
#include "../audio/core/plugins.h"

namespace Audio {
    class PluginDescriptor;
}

namespace Vst {

class VstHost;

class ScanThread;

class PluginFinder : public QThread
{
    Q_OBJECT

public:
    PluginFinder();
    ~PluginFinder();
    void addPathToScan(QString path);
    void scan(Vst::VstHost *host);
    void clearScanPaths();

private:
    void run();
signals:
    void scanStarted();
    void scanFinished();
    void vstPluginFounded(QString name, QString group, QString path);
private:
    QList<QString> scanPaths;
    Audio::PluginDescriptor getPluginDescriptor(QFileInfo f, Vst::VstHost* host);
    Vst::VstHost* host;
};

}

