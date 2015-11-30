#include "VstLoader.h"
#include <QApplication>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QDir>
#include <QLibrary>
#include "VstHost.h"
#include "log/logging.h"

using namespace Vst;

AEffect* VstLoader::load(QString path, Vst::Host* host){
    if(!host){
        return 0;
    }
    QString pluginDir = QFileInfo(path).absoluteDir().absolutePath();
    QApplication::addLibraryPath(pluginDir);

    QLibrary pluginLib(path);
    AEffect* effect = 0;

    // Plugin's entry point
    typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);
    vstPluginFuncPtr entryPoint=0;

    try {
        qCDebug(jtStandaloneVstPlugin) << "loading " << path;
        if(!pluginLib.load()){
            qCCritical(jtStandaloneVstPlugin) << "error when loading VST plugin " << path << " -> " << pluginLib.errorString();
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
        qCCritical(jtStandaloneVstPlugin) << "exception when  getting entry point " << pluginLib.fileName();
    }
    if(!entryPoint) {
        qCDebug(jtStandaloneVstPlugin) << "Entry point not founded, unloading plugin " << path ;
        return 0;
    }
    qCDebug(jtStandaloneVstPlugin) << "Entry point founded for " << path ;
    QApplication::processEvents();
    try{
        qCDebug(jtStandaloneVstPlugin) << "Initializing effect for " << path ;
        effect = entryPoint( (audioMasterCallback)host->hostCallback);// myHost->vstHost->AudioMasterCallback);
    }
    catch(... ){
        qCCritical(jtStandaloneVstPlugin) << "Error loading VST plugin";
        effect = 0;
    }

    if(!effect) {
        qCCritical(jtStandaloneVstPlugin) << "Error when initializing effect. Unloading " << path ;
        return 0;
    }
    QApplication::processEvents();
    if (effect->magic != kEffectMagic) {
        qCCritical(jtStandaloneVstPlugin) << "KEffectMagic error for " << path ;
        return 0;
    }
    return effect;
}

void VstLoader::unload(AEffect *effect){
    if(effect){
        effect->dispatcher(effect, effClose, 0, 0, NULL, 0);
    }
}
