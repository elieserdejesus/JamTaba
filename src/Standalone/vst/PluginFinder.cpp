#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "log/Logging.h"

#include <QApplication>
#include <QLibraryInfo>

using namespace audio;

VSTPluginFinder::VSTPluginFinder()
{
}

VSTPluginFinder::~VSTPluginFinder()
{
}

void VSTPluginFinder::setFoldersToScan(const QStringList &folders)
{
    scanFolders.clear();
    scanFolders.append(folders);
}

Audio::PluginDescriptor VSTPluginFinder::getPluginDescriptor(const QFileInfo &f)
{
    QString name = Audio::PluginDescriptor::getVstPluginNameFromPath(f.absoluteFilePath());
    Audio::PluginDescriptor::Category category = Audio::PluginDescriptor::VST_Plugin;
    return Audio::PluginDescriptor(name, category, f.absoluteFilePath());
}

void VSTPluginFinder::finishScan()
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

void VSTPluginFinder::handleScanError(QProcess::ProcessError error)
{
    qCritical(jtStandalonePluginFinder) << "ERROR:" << error << scanProcess.errorString();
    finishScan();
}

void VSTPluginFinder::handleProcessError(const QString &lastScannedPlugin)
{
    if (!lastScannedPlugin.isEmpty())
        emit badPluginDetected(lastScannedPlugin);
}

QString VSTPluginFinder::getScannerExecutablePath() const
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

void VSTPluginFinder::consumeOutputFromScanProcess()
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
                    QString pluginName = Audio::PluginDescriptor::getVstPluginNameFromPath(pluginPath);
                    emit pluginScanFinished(pluginName, pluginPath, "VST");
                }
            }
        }
    }
}

QString VSTPluginFinder::buildCommaSeparetedString(const QStringList &list) const
{
    QString folderString;
    for (int c = 0; c < list.size(); ++c) {
        folderString += list.at(c);
        if (c < list.size() -1)
            folderString += ";";
    }
    return folderString;
}

void VSTPluginFinder::scan(const QStringList &skipList)
{
    if (scanProcess.isOpen()) {
        qCritical() << "scan process is already open!";
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

void VSTPluginFinder::cancel()
{
    if (scanProcess.isOpen())
        scanProcess.terminate();
}
