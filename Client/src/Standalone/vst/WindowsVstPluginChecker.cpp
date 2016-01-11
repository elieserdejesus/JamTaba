#include "VstPluginChecker.h"
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QDataStream>

class ExecutableFormatChecker
{
public:
    static bool is32Bits(const QString &dllPath);
    static bool is64Bits(const QString &dllPath);

private:
    static quint16 getMachineHeader(const QString &dllPath);
};


//implementation for the windows version of this function. Another version is implemented in the MacVstPluginChecker.cpp file.
bool Vst::PluginChecker::isValidPluginFile(const QString &pluginPath)
{
    if(!QFileInfo(pluginPath).isFile())
        return false;

    if(!QLibrary::isLibrary(pluginPath))
        return false;

    if(QFileInfo(pluginPath).fileName().contains("Jamtaba"))//avoid Jamtaba standalone loading Jamtaba plugin. This is just a basic check, when the VSt plugin is loaded the real (compiled) name of the plugin is checked again.
        return false;

#ifdef _WIN64
    return ExecutableFormatChecker::is64Bits(pluginPath);
#else
    return WindowsDllArchChecker::is32Bits(pluginPath);
#endif

    return false;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

bool ExecutableFormatChecker::is32Bits(const QString &dllPath)
{
    return getMachineHeader(dllPath) == 0x14c;// i386
}

bool ExecutableFormatChecker::is64Bits(const QString &dllPath)
{
    return getMachineHeader(dllPath) == 0x8664;// AMD64
}

quint16 ExecutableFormatChecker::getMachineHeader(const QString &dllPath)
{
    // See http://www.microsoft.com/whdc/system/platform/firmware/PECOFF.mspx
    // Offset to PE header is always at 0x3C.
    // The PE header starts with "PE\0\0" =  0x50 0x45 0x00 0x00,
    // followed by a 2-byte machine type field (see the document above for the enum).

    QFile dllFile(dllPath);
    if (!dllFile.open(QFile::ReadOnly))
        return 0;
    dllFile.seek(0x3c);
    QDataStream dataStream(&dllFile);
    dataStream.setByteOrder(QDataStream::LittleEndian);
    qint32 peOffset;
    dataStream >> peOffset;
    dllFile.seek(peOffset);
    quint32 peHead;
    dataStream >> peHead;
    if (peHead != 0x00004550) // "PE\0\0", little-endian
        return 0; // "Can't find PE header"

    quint16 machineType;
    dataStream >> machineType;
    dllFile.close();
    return machineType;
}
