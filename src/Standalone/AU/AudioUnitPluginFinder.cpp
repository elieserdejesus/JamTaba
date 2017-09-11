#include "AudioUnitPluginFinder.h"

using namespace audio;

#include <QApplication>
#include "log/Logging.h"

AudioUnitPluginFinder::AudioUnitPluginFinder()
{

}


QString AudioUnitPluginFinder::getScannerExecutablePath() const
{
    // try the same jamtaba executable path first
    QString scannerExePath = QApplication::applicationDirPath() + "/AUScanner";// In the deployed and debug version the AUScanner and Jamtaba2 executables are in the same folder.
    if (QFile(scannerExePath).exists())
        return scannerExePath;
    else
        qCritical() << "Scanner executable not founded in" << scannerExePath;
    return "";
}

void AudioUnitPluginFinder::handleScanningStart(const QString &scannedLine)
{
    Q_UNUSED(scannedLine);
}

void AudioUnitPluginFinder::handleScanningFinished(const QString &scannedLine)
{
    //qCritical() << "scanned line:" << scannedLine;
    QStringList parts = scannedLine.split(": ");
    if (parts.size() < 3) {
        qCritical() << "Missing parts in scanned line: " << scannedLine;
        return;
    }

    QString pluginRawData = parts.at(1) + ": " + parts.at(2);
    //qDebug() << "rawData:" << pluginRawData;

    QStringList pluginRawDataParts = pluginRawData.split(";");
    if (pluginRawDataParts.size() < 2) {
        qCritical() << "Missing parts in plugin raw data: " << pluginRawData;
        return;
    }

    QString pluginPath = pluginRawDataParts.at(1).trimmed();
    QString pluginName = pluginRawDataParts.at(0).trimmed();

    emit pluginScanFinished(pluginName, pluginPath);
}
