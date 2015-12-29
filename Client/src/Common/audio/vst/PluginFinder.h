#ifndef PLUGIN_FINDER_H
#define PLUGIN_FINDER_H

#include <QObject>
//#include <QStringList>

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

private slots: // TODO better names for these slots. Remove the "on_anything..."
    void on_processStandardOutputReady();
    void on_processFinished();
    void on_processError(QProcess::ProcessError);

signals:
    void scanStarted();
    void scanFinished(bool finishedWithoutError);
    void pluginScanStarted(QString path);
    void pluginScanFinished(QString name, QString group, QString path);
    void badPluginDetected(QString pluginPath);// a plugin crash the scanner process
};
}

#endif
