#include "VstPluginScanner.h"
#include "audio/core/PluginDescriptor.h"
#include "vst/VstHost.h"
#include "vst/VstLoader.h"
#include "vst/Utils.h"
#include "VstPluginChecker.h"
#include "log/Logging.h"

#include <QDataStream>
#include <QDirIterator>
#include <QLibrary>

VstPluginScanner::VstPluginScanner()
    : BaseScanner()
{
    qCDebug(jtStandalonePluginFinder) << "Creating vst plugin scanner!";
}

Audio::PluginDescriptor VstPluginScanner::getPluginDescriptor(const QFileInfo &pluginFile)
{
    try{
        // PluginChecker is implemented in WindowsVstPluginChecker and MacVstPluginChecker files. Only the correct file is include in VstScanner.pro
        if (!Vst::PluginChecker::isValidPluginFile(pluginFile.absoluteFilePath()))
            return Audio::PluginDescriptor(); // invalid descriptor

        Vst::VstHost *host = Vst::VstHost::getInstance();
        AEffect *effect = Vst::VstLoader::load(pluginFile.absoluteFilePath(), host);
        if (effect) {
            QString name = Audio::PluginDescriptor::getVstPluginNameFromPath(pluginFile.absoluteFilePath());
            QString manufacturer = Vst::utils::getPluginVendor(effect);
            Vst::VstLoader::unload(effect); // delete the AEffect instance
            QLibrary lib(pluginFile.absoluteFilePath());
            lib.unload(); // try unload the shared lib

            return Audio::PluginDescriptor(name, Audio::PluginDescriptor::VST_Plugin, manufacturer, pluginFile.absoluteFilePath());
        }
    }
    catch (...) {
        qCritical() << "Error loading " << pluginFile.absoluteFilePath();
    }
    return Audio::PluginDescriptor(); // invalid descriptor
}

void VstPluginScanner::scan()
{
    if (foldersToScan.isEmpty()) {
        qCInfo(jtStandalonePluginFinder) << "Folders to scan is empty!";
        return;
    } else {
        qCDebug(jtStandalonePluginFinder) << "Folders to scan: " << foldersToScan;
    }

    writeToProcessOutput("JT-Scanner-Starting");
    for (const QString &scanFolder : foldersToScan)
    {
        QDirIterator folderIterator(scanFolder, QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDirIterator::Subdirectories);
        while (folderIterator.hasNext())
        {
            folderIterator.next(); // point to next file inside current folder
            QFileInfo pluginFileInfo(folderIterator.filePath());

            if (!skipList.contains(pluginFileInfo.absoluteFilePath()))
            {
                if (canScan(pluginFileInfo)) {
                    writeToProcessOutput("JT-Scanner-Scanning: "+ pluginFileInfo.absoluteFilePath());
                    auto descriptor = getPluginDescriptor(pluginFileInfo);
                    if (descriptor.isValid())
                        writeToProcessOutput("JT-Scanner-Scan-Finished: " + descriptor.getPath());
                }
            }
        }
    }
    writeToProcessOutput("JT-Scanner-Finished");
}


bool VstPluginScanner::canScan(const QFileInfo &pluginFileInfo) const
{
    /**
      In Mac VST plugins are bundles, in windows these plugins are DLLs.
    */

    return pluginFileInfo.isBundle() || pluginFileInfo.suffix() == "dll";
}

void VstPluginScanner::initialize(int argc, char *argv[])
{
    /**
     The first arg is always the executable path. We need at least the folders strinb (2nd arg).
     The blacklist can be empty.
    */

    qCInfo(jtStandalonePluginFinder) << "Initializing scan folders list and blackList!";

    if (argc < 2)
        return;

    QString foldersString = QString::fromUtf8(argv[1]);

    if (!foldersString.isEmpty())
        this->foldersToScan = foldersString.split(";"); // the folders are separated using ';'

    if (argc > 2) { // we have a black list string?
        QString skipListString = QString::fromUtf8(argv[2]);
        if (!skipListString.isEmpty())
            this->skipList = skipListString.split(";");
    }
}
