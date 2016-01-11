#include "VstPluginScanner.h"
#include <iostream>
#include <QDataStream>
#include <QDirIterator>
#include <QLibrary>
#include "audio/core/PluginDescriptor.h"
#include "vst/VstHost.h"
#include "vst/VstLoader.h"
#include "log/Logging.h"
#include "VstPluginChecker.h"

VstPluginScanner::VstPluginScanner() :
    QObject()
{
    qCDebug(jtStandalonePluginFinder) << "Creating vst plugin scanner!";
}

void VstPluginScanner::start(int argc, char *argv[])
{
    initialize(argc, argv);// get the folders to scan and black listed plugin paths
    scan();
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
    foreach (const QString &scanFolder, foldersToScan) {
        QDirIterator folderIterator(scanFolder, QDirIterator::Subdirectories);
        while (folderIterator.hasNext()) {
            folderIterator.next();// point to next file inside current folder
            QFileInfo pluginFileInfo(folderIterator.filePath());
            if (!skipList.contains(pluginFileInfo.absoluteFilePath())) {
                writeToProcessOutput("JT-Scanner-Scanning: "+ pluginFileInfo.absoluteFilePath());
                const Audio::PluginDescriptor &descriptor = getPluginDescriptor(pluginFileInfo);
                if (descriptor.isValid())
                    writeToProcessOutput("JT-Scanner-Scan-Finished: " + descriptor.getPath());
            }
        }
    }
    writeToProcessOutput("JT-Scanner-Finished");
}

Audio::PluginDescriptor VstPluginScanner::getPluginDescriptor(const QFileInfo &pluginFile)
{
    try{
        //PluginChecker is implemented in WindowsVstPluginChecker and MacVstPluginChecker files. Only the correct file is include in VstScanner.pro
        if (!Vst::PluginChecker::isValidPluginFile(pluginFile.absoluteFilePath()))
            return Audio::PluginDescriptor();// invalid descriptor

        AEffect *effect = Vst::VstLoader::load(pluginFile.absoluteFilePath(), Vst::Host::getInstance());
        if (effect) {
            QString name = Audio::PluginDescriptor::getPluginNameFromPath(pluginFile.absoluteFilePath());
            Vst::VstLoader::unload(effect);// delete the AEffect instance
            QLibrary lib(pluginFile.absoluteFilePath());
            lib.unload();// try unload the shared lib
            return Audio::PluginDescriptor(name, "VST", pluginFile.absoluteFilePath());
        }
    }
    catch (...) {
        qCritical() << "Error loading " << pluginFile.absoluteFilePath();
    }
    return Audio::PluginDescriptor();// invalid descriptor
}

void VstPluginScanner::writeToProcessOutput(const QString &string)
{
    // using '\n' here because std::endl don't work well when reading the output from QProcess
    std::cout << '\n' << string.toStdString() << '\n';
    std::flush(std::cout);// necessary to avoid split some outputed lines
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
    if (argc > 2) {// we have a black list string?
        QString skipListString = QString::fromUtf8(argv[2]);
        if (!skipListString.isEmpty())
            this->skipList = skipListString.split(";");
    }
}
