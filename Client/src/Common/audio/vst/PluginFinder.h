#ifndef PLUGIN_FINDER_H
#define PLUGIN_FINDER_H

#include <QObject>
#include <QProcess>

namespace Audio {
class PluginDescriptor;
}

namespace Vst {

class PluginFinder : public QObject
{
    Q_OBJECT
public:
    PluginFinder();
    virtual ~PluginFinder();
    void scan(QStringList skipList);
    void setFoldersToScan(QStringList folders);
    void cancel();
private:
    QStringList scanFolders;
    QProcess scanProcess;
    QString lastScannedPlugin;// used to recover the last plugin path when the scanner process crash

    Audio::PluginDescriptor getPluginDescriptor(QFileInfo f);
    QString buildCommaSeparetedString(QStringList list) const;
    QString getVstScannerExecutablePath() const;
    void handleProcessError(QString lastScannedPlugin);

private slots:
    void consumeOutputFromScanProcess();
    void finishScan();
    void handleScanError(QProcess::ProcessError);

signals:
    void scanStarted();
    void scanFinished(bool finishedWithoutError);
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
    void badPluginDetected(QString pluginPath);// a plugin crash the scanner process
};
}

#endif
