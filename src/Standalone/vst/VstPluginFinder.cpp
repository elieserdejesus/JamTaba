#include "VstPluginFinder.h"

#include <QApplication>
#include <QLibraryInfo>

#include "log/Logging.h"

using namespace audio;

VSTPluginFinder::VSTPluginFinder()
{
    //
}

VSTPluginFinder::~VSTPluginFinder()
{
    //
}

audio::PluginDescriptor VSTPluginFinder::getPluginDescriptor(const QFileInfo &f)
{
    QString name = audio::PluginDescriptor::getVstPluginNameFromPath(f.absoluteFilePath());
    auto category = audio::PluginDescriptor::VST_Plugin;
    return audio::PluginDescriptor(name, category, f.absoluteFilePath());
}

QString VSTPluginFinder::getScannerExecutablePath() const
{
    // try the same jamtaba executable path first
    QString scannerExePath = QApplication::applicationDirPath() + "/VstScanner"; // In the deployed and debug version the VstScanner and Jamtaba2 executables are in the same folder.
#ifdef Q_OS_WIN
    scannerExePath += ".exe";
#endif
    if (QFile(scannerExePath).exists())
        return scannerExePath;
    else
        qCritical() << "Scanner executable not founded in" << scannerExePath;
    return "";
}

void VSTPluginFinder::handleScanningStart(const QString &scannedLine)
{
    QStringList parts = scannedLine.split(": ");
    if (parts.size() < 2) {
        qCritical() << "Missing parts in scanned line: " << scannedLine;
        return;
    }

    QString pluginPath = parts.at(1);
    lastScannedPlugin = pluginPath;// store the plugin path, if the scanner process crash we can add this bad plugin in the black list
    emit pluginScanStarted(pluginPath);
}

void VSTPluginFinder::handleScanningFinished(const QString &scannedLine)
{
    QStringList parts = scannedLine.split(": ");
    if (parts.size() < 2) {
        qCritical() << "Missing parts in scanned line: " << scannedLine;
        return;
    }

    QString pluginPath = parts.at(1);
    QString pluginName = audio::PluginDescriptor::getVstPluginNameFromPath(pluginPath);
    emit pluginScanFinished(pluginName, pluginPath);
}
