#pragma once
#include <QFileInfo>
#include <QProcess>
#include "audio/core/plugins.h"

namespace Audio {
    class PluginDescriptor;
}

namespace Vst {

class Host;

class PluginFinder : public QObject
{
    Q_OBJECT
public:
    PluginFinder();
    virtual void scan(QStringList blackList) = 0;
    void addFolderToScan(QString path);
    void clearScanFolders();
    virtual void cancel() = 0;
protected:
    QList<QString> scanFolders;
signals:
    void scanStarted();
    void scanFinished();
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
    void badPluginDetected(QString pluginPath);//a plugin crash the scanner process
};


}

