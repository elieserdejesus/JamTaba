#ifndef PLUGIN_FINDER_H
#define PLUGIN_FINDER_H

#include <QObject>
#include <QProcess>
#include <QFileInfo>

namespace Audio {
class PluginDescriptor;
}

namespace audio {

class PluginFinder : public QObject
{
    Q_OBJECT
public:
    PluginFinder();
    virtual ~PluginFinder();
    void scan(const QStringList &skipList);
    void setFoldersToScan(const QStringList &folders);
    void cancel();
private:
    QStringList scanFolders;
    QProcess scanProcess;
    QString lastScannedPlugin;// used to recover the last plugin path when the scanner process crash

    Audio::PluginDescriptor getPluginDescriptor(const QFileInfo &f);
    QString buildCommaSeparetedString(const QStringList &list) const;
    QString getScannerExecutablePath() const;
    void handleProcessError(const QString &lastScannedPlugin);

private slots:
    void consumeOutputFromScanProcess();
    void finishScan();
    void handleScanError(QProcess::ProcessError);

signals:
    void scanStarted();
    void scanFinished(bool finishedWithoutError);
    void pluginScanStarted(const QString &path);
    void pluginScanFinished(const QString &name, const QString &group, const QString &path);
    void badPluginDetected(const QString &pluginPath);// a plugin crashed the scanner process
};
}

#endif
