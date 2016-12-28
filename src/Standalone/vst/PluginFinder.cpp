#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "log/Logging.h"

#include <QApplication>
#include <QLibraryInfo>

using namespace audio;

PluginFinder::PluginFinder()
{
}

PluginFinder::~PluginFinder()
{
}

void PluginFinder::setFoldersToScan(const QStringList &folders)
{
    scanFolders.clear();
    scanFolders.append(folders);
}

Audio::PluginDescriptor PluginFinder::getPluginDescriptor(const QFileInfo &f)
{
    QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
    Audio::PluginDescriptor::Category category = Audio::PluginDescriptor::VST_Plugin;
    return Audio::PluginDescriptor(name, category, f.absoluteFilePath());
}

void PluginFinder::finishScan()
{
    QProcess::ExitStatus exitStatus = scanProcess.exitStatus();
    scanProcess.close();
    bool exitingWithoutError = exitStatus == QProcess::NormalExit;
    emit scanFinished(exitingWithoutError);
    QString lastScanned(lastScannedPlugin);
    lastScannedPlugin.clear();
    if (!exitingWithoutError)
        handleProcessError(lastScanned);
}

void PluginFinder::handleScanError(QProcess::ProcessError error)
{
    qCritical(jtStandalonePluginFinder) << "ERROR:" << error << scanProcess.errorString();
    finishScan();
}

void PluginFinder::handleProcessError(const QString &lastScannedPlugin)
{
    if (!lastScannedPlugin.isEmpty())
        emit badPluginDetected(lastScannedPlugin);
}

QString PluginFinder::getScannerExecutablePath() const
{
    // try the same jamtaba executable path first
    QString scannerExePath = QApplication::applicationDirPath() + "/VstScanner";// In the deployed and debug version the VstScanner and Jamtaba2 executables are in the same folder.
#ifdef Q_OS_WIN
    scannerExePath += ".exe";
#endif
    if (QFile(scannerExePath).exists())
        return scannerExePath;
    else
        qCritical() << "Scanner executable not founded in" << scannerExePath;
    return "";
}

void PluginFinder::consumeOutputFromScanProcess()
{
    QByteArray readedData = scanProcess.readAll();
    QTextStream stream(readedData, QIODevice::ReadOnly);
    while (!stream.atEnd()) {
        QString readedLine = stream.readLine();
        if (!readedLine.isNull() && !readedLine.isEmpty()) {
            bool startScanning = readedLine.startsWith("JT-Scanner-Scanning:");
            bool finishedScanning = readedLine.startsWith("JT-Scanner-Scan-Finished");
            if (startScanning || finishedScanning) {
                QString pluginPath = readedLine.split(": ").at(1);
                if (startScanning) {
                    lastScannedPlugin = pluginPath;// store the plugin path, if the scanner process crash we can add this bad plugin in the black list
                    emit pluginScanStarted(pluginPath);
                } else {
                    QString pluginName = Audio::PluginDescriptor::getPluginNameFromPath(pluginPath);
                    emit pluginScanFinished(pluginName, "VST", pluginPath);
                }
            }
        }
    }
}

QString PluginFinder::buildCommaSeparetedString(const QStringList &list) const
{
    QString folderString;
    for (int c = 0; c < list.size(); ++c) {
        folderString += list.at(c);
        if (c < list.size() -1)
            folderString += ";";
    }
    return folderString;
}

void PluginFinder::scan(const QStringList &skipList)
{
    if (scanProcess.isOpen()) {
        qCWarning(jtStandalonePluginFinder) << "scan process is already open!";
        return;
    }

    QString scannerExePath = getScannerExecutablePath();
    if (scannerExePath.isEmpty())
        return;// scanner executable not found!

    emit scanStarted();
    // execute the scanner in another process to avoid crash Jamtaba process
    QStringList parameters;
    parameters.append(buildCommaSeparetedString(scanFolders));
    parameters.append(buildCommaSeparetedString(skipList));
    QObject::connect(&scanProcess, SIGNAL(readyReadStandardOutput()), this,
                     SLOT(consumeOutputFromScanProcess()));
    QObject::connect(&scanProcess, SIGNAL(finished(int)), this, SLOT(finishScan()));
    QObject::connect(&scanProcess, SIGNAL(error(QProcess::ProcessError)), this,
                     SLOT(handleScanError(QProcess::ProcessError)));
    qCDebug(jtStandalonePluginFinder) << "Starting scan process...";
    scanProcess.start(scannerExePath, parameters);
    qCDebug(jtStandalonePluginFinder) << "Scan process started with " << scannerExePath;
}

void PluginFinder::cancel()
{
    if (scanProcess.isOpen())
        scanProcess.terminate();
}
