#pragma once
#include <QFileInfo>
#include <QThread>
#include "../audio/core/plugins.h"

namespace Audio {
    class PluginDescriptor;
}

namespace Vst {

class Host;

class PluginFinder : public QObject
{
    Q_OBJECT
public:
    virtual void scan() = 0;
    void addPathToScan(QString path);
    void clearScanPaths();
protected:
    QList<QString> scanPaths;
signals:
    void scanStarted();
    void scanFinished();
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
};


}

