#pragma once
#include <QObject>
#include <QStringList>

namespace Audio {
class PluginDescriptor;
}

namespace Vst {
class PluginFinder : public QObject
{
    Q_OBJECT
public:
    PluginFinder();
    virtual void scan(QStringList skipList) = 0;
    void setFoldersToScan(QStringList folders);
    virtual void cancel() = 0;
protected:
    QStringList scanFolders;
signals:
    void scanStarted();
    void scanFinished(bool finishedWithoutError);
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
    void badPluginDetected(QString pluginPath);// a plugin crash the scanner process
};
}
