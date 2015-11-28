#include "VstPluginScanner.h"
#include "audio/core/PluginDescriptor.h"
#include <QFileInfo>
#include <QFile>
#include <QLibrary>
#include <QDebug>
#include "audio/vst/VstLoader.h"
#include "audio/vst/VstHost.h"

Audio::PluginDescriptor VstPluginScanner::getPluginDescriptor(QFileInfo f){

    try{
        if(!f.isBundle() || f.completeSuffix() != "vst"){
            return Audio::PluginDescriptor();//invalid descriptor
        }
        if( Vst::VstLoader::load(f.absoluteFilePath(), Vst::Host::getInstance()) ){
            QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
            return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
        }
    }
    catch(...){
        qCritical() << "Error loading " << f.absoluteFilePath();
    }
    return Audio::PluginDescriptor();//invalid descriptor
}
