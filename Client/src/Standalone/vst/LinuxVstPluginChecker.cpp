#include "VstPluginChecker.h"
#include <QFile>
#include <QFileInfo>
#include <QLibrary>

//implementation for the Linux version of this function. Another version are implemented in the MacVstPluginChecker.cpp and WindowsVstPluginChecker files.
bool Vst::PluginChecker::isValidPluginFile(const QString &pluginPath)
{
    if(!QFileInfo(pluginPath).isFile())
        return false;

    if(!QLibrary::isLibrary(pluginPath))
        return false;

    if(QFileInfo(pluginPath).fileName().contains("Jamtaba"))//avoid Jamtaba standalone loading Jamtaba plugin. This is just a basic check, when the VSt plugin is loaded the real (compiled) name of the plugin is checked again.
        return false;

   return false;
}
