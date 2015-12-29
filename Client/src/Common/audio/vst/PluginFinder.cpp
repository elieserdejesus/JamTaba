#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "log/Logging.h"

using namespace Vst;

PluginFinder::PluginFinder()
{
}

PluginFinder::~PluginFinder()
{
}

void PluginFinder::setFoldersToScan(QStringList folders)
{
    scanFolders.clear();
    scanFolders.append(folders);
}

Audio::PluginDescriptor PluginFinder::getPluginDescriptor(QFileInfo f)
{
    QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
    return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
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

void PluginFinder::handleProcessError(QString lastScannedPlugin)
{
    if (!lastScannedPlugin.isEmpty())
        emit badPluginDetected(lastScannedPlugin);
}

QString PluginFinder::getVstScannerExecutablePath() const
{
    // try the same jamtaba executable path first
    QString scannerExePath = QApplication::applicationDirPath() + "/VstScanner";// In the deployed version the VstScanner and Jamtaba2 executables are in the same folder.
#ifdef Q_OS_WIN
    scannerExePath += ".exe";
#endif
    if (QFile(scannerExePath).exists())
        return scannerExePath;
    else
        qWarning(jtStandalonePluginFinder) << "Scanner exe not founded in" << scannerExePath;

    // In dev time the executable (Jamtaba2 and VstScanner) are in different folders...
    // We need a more elegant way to solve this at dev time. At moment I'm a very dirst approach to return the executable path in MacOsx, and just a little less dirty solution in windows.
    QString appPath = QCoreApplication::applicationDirPath();
#ifdef Q_OS_MAC
    return
        "/Users/elieser/Desktop/build-Jamtaba-Desktop_Qt_5_5_0_clang_64bit-Debug/VstScanner/VstScanner";
#endif
    QString buildType = QLibraryInfo::isDebugBuild() ? "debug" : "release";
    scannerExePath = appPath + "/../../VstScanner/"+ buildType +"/VstScanner.exe";
    if (QFile(scannerExePath).exists())
        return scannerExePath;
    qCCritical(jtStandalonePluginFinder) << "Vst scanner exeutable not found in" << scannerExePath;
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

QString PluginFinder::buildCommaSeparetedString(QStringList list) const
{
    QString folderString;
    for (int c = 0; c < list.size(); ++c) {
        folderString += list.at(c);
        if (c < list.size() -1)
            folderString += ";";
    }
    return folderString;
}

void PluginFinder::scan(QStringList skipList)
{
    if (scanProcess.isOpen()) {
        qCWarning(jtStandalonePluginFinder) << "scan process is already open!";
        return;
    }

    QString scannerExePath = getVstScannerExecutablePath();
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
