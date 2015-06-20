#include "PluginFinder.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include "../audio/core/plugins.h"
#include "windows.h"
#include "VstPlugin.h"
#include "vsthost.h"
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
            Audio::PluginDescriptor* descriptor = getPluginDescriptor(fileInfo, host);
            if(descriptor){
                emit vstPluginFounded(descriptor);
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

//retorna nullptr se não for um plugin
Audio::PluginDescriptor* PluginFinder::getPluginDescriptor(QFileInfo f, Vst::VstHost* host){
    if (!f.isFile() || f.suffix() != "dll")
        return nullptr;

    try{
        Vst::VstPlugin plugin(host);
        if(plugin.load(host, f.absoluteFilePath())){
            QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
            return new Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
        }
        return nullptr;
    }
    catch(...){
        qCritical() << "não carregou " << f.absoluteFilePath();
    }
    return nullptr;
}

void PluginFinder::addPathToScan(QString path){
    scanPaths.push_back(path);
}
