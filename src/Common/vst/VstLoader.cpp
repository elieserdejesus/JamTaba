#include "VstLoader.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QDir>
#include <QLibrary>
#include "VstHost.h"
#include "log/Logging.h"

using namespace vst;

AEffect* VstLoader::load(const QString &path, vst::VstHost* host){
    if(!host){
        return 0;
    }
    QString pluginDir = QFileInfo(path).absoluteDir().absolutePath();
    QCoreApplication::addLibraryPath(pluginDir);

    QLibrary pluginLib(path);
    AEffect* effect = 0;

    // Plugin's entry point
    typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);
    vstPluginFuncPtr entryPoint=0;

    try {
        qCDebug(jtStandaloneVstPlugin) << "loading " << path;
        if(!pluginLib.load()){
            qCritical() << "error when loading VST plugin " << path << " -> " << pluginLib.errorString();
            return 0;
        }
        qCDebug(jtStandaloneVstPlugin) << path << " loaded";
        qCDebug(jtStandaloneVstPlugin) << "searching VST plugin entry point for" << path;
        entryPoint = (vstPluginFuncPtr) pluginLib.resolve("VSTPluginMain");
        if(!entryPoint){
            entryPoint = (vstPluginFuncPtr)pluginLib.resolve("main");
        }
    }
    catch(...){
        qCritical() << "exception when  getting entry point " << pluginLib.fileName();
    }
    if(!entryPoint) {
        qCDebug(jtStandaloneVstPlugin) << "Entry point not founded, unloading plugin " << path ;
        return 0;
    }
    qCDebug(jtStandaloneVstPlugin) << "Entry point founded for " << path ;
    //QCoreApplication::processEvents();
    try{
        qCDebug(jtStandaloneVstPlugin) << "Initializing effect for " << path ;
        effect = entryPoint((audioMasterCallback)host->hostCallback);// myHost->vstHost->AudioMasterCallback);
    }
    catch(... ){
        qCritical() << "Error loading VST plugin";
        effect = 0;
    }

    if(!effect) {
        qCritical() << "Error when initializing effect. Unloading " << path ;
        return 0;
    }
    //QCoreApplication::processEvents();
    if (effect->magic != kEffectMagic) {
        qCritical() << "KEffectMagic error for " << path ;
        return 0;
    }
    return effect;
}

void VstLoader::unload(AEffect *effect){
    if(effect){
        effect->dispatcher(effect, effClose, 0, 0, NULL, 0);
    }
}
