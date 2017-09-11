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
    void scan(const QStringList &foldersToScan = QStringList(), const QStringList &skipList = QStringList());
    void cancel();

protected:
    QProcess scanProcess;
    QString lastScannedPlugin;

    virtual QString getScannerExecutablePath() const = 0;

    void handleProcessError(const QString &lastScannedPlugin);

    virtual void handleScanningStart(const QString &scannedLine) = 0;
    virtual void handleScanningFinished(const QString &scannedLine) = 0;

     QString buildCommaSeparatedString(const QStringList &list) const;

protected slots:
    void consumeOutputFromScanProcess();
    void finishScan();
    void handleScanError(QProcess::ProcessError);

signals:
    void scanStarted();
    void scanFinished(bool finishedWithoutError);
    void pluginScanStarted(const QString &path);
    void pluginScanFinished(const QString &name, const QString &path);
    void badPluginDetected(const QString &pluginPath); // a plugin crashed the scanner process

};

} // namespace

#endif
