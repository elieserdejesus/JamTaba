#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"
#include "log/Logging.h"

using namespace audio;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void PluginFinder::finishScan()
{
    QProcess::ExitStatus exitStatus = scanProcess.exitStatus();
    scanProcess.close();
    bool exitingWithoutError = exitStatus == QProcess::NormalExit;

    qCDebug(jtStandalonePluginFinder) << "Closing scan process! exited without error:" << exitingWithoutError;

    emit scanFinished(exitingWithoutError);
    QString lastScanned(lastScannedPlugin);
    lastScannedPlugin.clear();
    if (!exitingWithoutError)
        handleProcessError(lastScanned);

}

void PluginFinder::handleScanError(QProcess::ProcessError error)
{
    qCritical() << error << scanProcess.errorString();
    finishScan();
}

void PluginFinder::handleProcessError(const QString &lastScannedPlugin)
{
    if (!lastScannedPlugin.isEmpty())
        emit badPluginDetected(lastScannedPlugin);
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
            if (startScanning)
                handleScanningStart(readedLine);
            else if(finishedScanning)
                handleScanningFinished(readedLine);
        }
    }
}


void PluginFinder::scan(const QStringList &scanFolders, const QStringList &skipList)
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
    parameters.append(buildCommaSeparatedString(scanFolders));
    parameters.append(buildCommaSeparatedString(skipList));
    QObject::connect(&scanProcess, SIGNAL(readyReadStandardOutput()), this,
                     SLOT(consumeOutputFromScanProcess()));
    QObject::connect(&scanProcess, SIGNAL(finished(int)), this, SLOT(finishScan()));
    QObject::connect(&scanProcess, SIGNAL(error(QProcess::ProcessError)), this,
                     SLOT(handleScanError(QProcess::ProcessError)));

    scanProcess.start(scannerExePath, parameters);
    qCDebug(jtStandalonePluginFinder) << "Scan process started with " << scannerExePath
                                          << " (PID: " << scanProcess.processId() << ")";
}

void PluginFinder::cancel()
{
    if (!scanProcess.atEnd()) {
        qCDebug(jtStandalonePluginFinder) << "Terminating scan process!";
        scanProcess.terminate();
        qDebug() << "process closed";
    }
}

QString PluginFinder::buildCommaSeparatedString(const QStringList &list) const
{
    QString folderString;
    for (int c = 0; c < list.size(); ++c) {
        folderString += list.at(c);
        if (c < list.size() -1)
            folderString += ";";
    }
    return folderString;
}

