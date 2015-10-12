#pragma once
#include <QFileInfo>
#include <QThread>
#include "../audio/core/plugins.h"

namespace Audio {
    class PluginDescriptor;
}

namespace Jamtaba {

class VstHost;

class ScanThread;

class PluginFinder : public QThread
{
    Q_OBJECT

public:
    PluginFinder();
    ~PluginFinder();
    void addPathToScan(QString path);
    void scan(Jamtaba::VstHost *host);
    void clearScanPaths();

private:
    void run();
signals:
    void scanStarted();
    void scanFinished();
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
private:
    QList<QString> scanPaths;
    Audio::PluginDescriptor getPluginDescriptor(QFileInfo f, Jamtaba::VstHost* host);
    Jamtaba::VstHost* host;
};

}

