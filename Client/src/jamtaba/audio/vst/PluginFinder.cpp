#include "PluginFinder.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include "../audio/core/plugins.h"
#if _WIN32
    #include "windows.h"
    #include "VstPlugin.h"
    #include "vsthost.h"
#endif
#include <QApplication>
#include <QSettings>

using namespace Vst;

PluginFinder::PluginFinder()
    :host(nullptr)
{

}

PluginFinder::~PluginFinder()
{

}


void PluginFinder::run(){
    emit scanStarted();
    for(QString scanPath : scanPaths){
        QDirIterator dirIt(scanPath, QDirIterator::Subdirectories);
        while (dirIt.hasNext()) {
            dirIt.next();
            QFileInfo fileInfo (dirIt.filePath());
            const Audio::PluginDescriptor& descriptor = getPluginDescriptor(fileInfo, host);
            if(descriptor.isValid()){
                emit vstPluginFounded(descriptor.getName(), descriptor.getGroup(), descriptor.getPath());
            }
            QApplication::processEvents();
        }
    }
    emit scanFinished();
}

void PluginFinder::scan(Vst::VstHost* host){
    this->host = host;
    if(!isRunning()){
        start();
        //run();
    }
}

void PluginFinder::clearScanPaths(){
    scanPaths.clear();
}


//typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

//extern "C" {
//// Main host callback
//  VstIntPtr VSTCALLBACK masterCallback(AEffect *effect, VstInt32 opcode,
//    VstInt32 index, VstInt32 value, void *ptr, float opt){
//        return 1;
//    }
//}

//++++++++++++++++++++++++++++++++++++++++++++
class WindowsDllArchChecker{
public:
    static bool is32Bits(QString dllPath){
        return getMachineHeader(dllPath) == 0x14c;//i386
    }

    static bool is64Bits(QString dllPath){
        return getMachineHeader(dllPath) == 0x8664;//AMD64
    }
private:
    static quint16 getMachineHeader(QString dllPath){
        // See http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
        // Offset to PE header is always at 0x3C.
        // The PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00,
        // followed by a 2-byte machine type field (see the document above for the enum).

        QFile dllFile(dllPath);
        if(!dllFile.open(QFile::ReadOnly)){
            return 0;
        }
        dllFile.seek(0x3c);
        QDataStream dataStream(&dllFile);
        dataStream.setByteOrder(QDataStream::LittleEndian);
        qint32 peOffset;
        dataStream >> peOffset;
        dllFile.seek(peOffset);
        quint32 peHead;
        dataStream >> peHead;
        if (peHead!=0x00004550) // "PE\0\0", little-endian
            return 0; //"Can't find PE header"

        quint16 machineType;
        dataStream >> machineType;
        dllFile.close();
        return machineType;
    }
};

//++++++++++++++++++++++++++++++++++++++++++++


//retorna nullptr se não for um plugin
Audio::PluginDescriptor PluginFinder::getPluginDescriptor(QFileInfo f, Vst::VstHost* host){
    if (!f.isFile() || f.suffix() != "dll")
        return Audio::PluginDescriptor();//invalid descriptor

    try{
        #if _WIN32
            if(WindowsDllArchChecker::is32Bits(f.absoluteFilePath())){
                Vst::VstPlugin plugin(host);
                if(plugin.load(host, f.absoluteFilePath())){
                    QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
                    return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
                }
            }
//            else{
//                qWarning() << "skipping 64 bits dll: " << f.fileName();
//            }
        #endif
    }
    catch(...){
        qCritical() << "não carregou " << f.absoluteFilePath();
    }
    return Audio::PluginDescriptor();//invalid descriptor
}

void PluginFinder::addPathToScan(QString path){
    if(!scanPaths.contains(path)){
        scanPaths.append(path);
    }
}
