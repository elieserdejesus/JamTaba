#include "AUPluginScanner.h"
#include <iostream>
#include <QDataStream>
#include <QDirIterator>
#include "audio/core/PluginDescriptor.h"
#include "log/Logging.h"

#include <CoreAudio/CoreAudio.h>
#include <AudioToolBox/AudioToolbox.h>
#include <AudioUnit/AudioUnit.h>
#include <CoreServices/CoreServices.h>



AUPluginScanner::AUPluginScanner() :
    QObject()
{
    qCDebug(jtStandalonePluginFinder) << "Creating Audio Unit (AU) plugin scanner!";
}

void AUPluginScanner::start(int argc, char *argv[])
{
    initialize(argc, argv);// get the folders to scan and black listed plugin paths
    scan();
}

void AUPluginScanner::scan()
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

            if (pluginFileInfo.suffix() == "component" && !skipList.contains(pluginFileInfo.absoluteFilePath())) {
                writeToProcessOutput("JT-Scanner-Scanning: "+ pluginFileInfo.absoluteFilePath());
                auto descriptor = getPluginDescriptor(pluginFileInfo);
                if (descriptor.isAU())
                    writeToProcessOutput("JT-Scanner-Scan-Finished: " + descriptor.getPath());
            }
        }
    }
    writeToProcessOutput("JT-Scanner-Finished");
}

Audio::PluginDescriptor AUPluginScanner::getPluginDescriptor(const QFileInfo &pluginFile)
{
    AUB

    AudioComponent comp = nullptr;
    do {
        AudioComponentDescription desc;
        desc.componentType = OSType(0);
        desc.componentSubType = OSType(0);
        desc.componentManufacturer = OSType(0);
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

       comp = AudioComponentFindNext(comp, &desc);
       if (comp) {
           AudioComponentInstance instance;
           OSStatus status = AudioComponentInstanceNew(comp, &instance);
           if (status == noErr) {
               status = AudioComponentGetDescription(comp, &desc);
               if (status == noErr) {
                   QString type(osTypeToString(desc.componentType));
                   QString subType(osTypeToString(desc.componentSubType));
                   QString manufacturer(desc.componentManufacturer);
                   qDebug() << "type: " << type << "sub: " << subType << "Man: " << manufacturer;
                   CFStringRef name;
                   status = AudioComponentCopyName(comp, &name);
                   if (status == noErr) {
                        qDebug() << "NAme: " << QString::fromCFString(name);
                   }
               }
               AudioComponentInstanceDispose(instance);
           }
       }
    }
    while(comp);

    return Audio::PluginDescriptor();// invalid descriptor
}

void AUPluginScanner::writeToProcessOutput(const QString &string)
{
    // using '\n' here because std::endl don't work well when reading the output from QProcess
    std::cout << '\n' << string.toStdString() << '\n';
    std::flush(std::cout);// necessary to avoid split some outputed lines
}

void AUPluginScanner::initialize(int argc, char *argv[])
{
    /**
     The first arg is always the executable path. We need at least the folders string (2nd arg).
     The blacklist (3rd string) can be empty.
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
