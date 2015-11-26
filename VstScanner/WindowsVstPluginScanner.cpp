#include "VstPluginScanner.h"
#include "audio/core/PluginDescriptor.h"
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QDataStream>
#include <QDebug>
#include "audio/vst/VstHost.h"
#include "audio/vst/VstLoader.h"

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
//+++++++++++++++++++++++++++++++++++
Audio::PluginDescriptor VstPluginScanner::getPluginDescriptor(QFileInfo f){

    try{
        bool archIsValid = true;
        bool isFile = f.isFile();
        bool isLibrary = QLibrary::isLibrary(f.fileName());
        bool isJamtabaVstPlugin = f.fileName().contains("Jamtaba");
        if (!isFile || !isLibrary || isJamtabaVstPlugin){
            return Audio::PluginDescriptor();//invalid descriptor
        }
#ifdef _WIN64
        archIsValid = WindowsDllArchChecker::is64Bits(f.absoluteFilePath());
#else
        archIsValid = WindowsDllArchChecker::is32Bits(f.absoluteFilePath());
#endif
        if(archIsValid){
            if( Vst::VstLoader::load(f.absoluteFilePath(), Vst::Host::getInstance()) ){
                QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
                return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
            }
        }
    }
    catch(...){
        qCritical() << "Error loading " << f.absoluteFilePath();
    }
    return Audio::PluginDescriptor();//invalid descriptor
}
