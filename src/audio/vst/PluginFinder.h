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
    PluginFinder();
    virtual void scan(QStringList blackList) = 0;
    void addFolderToScan(QString path);
    void clearScanFolders();
    void cancel();
protected:
    QList<QString> scanFolders;
    bool cancelRequested;
signals:
    void scanStarted();
    void scanFinished();
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
};


}

