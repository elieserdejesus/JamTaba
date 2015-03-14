#include "PluginFinder.h"
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include "../audio/core/plugins.h"
#include "windows.h"
#include "VstPlugin.h"
#include "vsthost.h"
#include <QApplication>

using namespace Vst;

PluginFinder::PluginFinder()
{

}

PluginFinder::~PluginFinder()
{

}

std::vector<Audio::PluginDescriptor*> PluginFinder::scan(Vst::VstHost* host){
    if(descriptorsCache.empty()){//scan
        for(std::string scanPath : scanPaths){
            QDirIterator dirIt(QString(scanPath.c_str()), QDirIterator::Subdirectories);
            while (dirIt.hasNext()) {
                dirIt.next();
                QFileInfo fileInfo (dirIt.filePath());
                Audio::PluginDescriptor* descriptor = getPluginDescriptor(fileInfo, host);
                if(descriptor){
                    descriptorsCache.push_back(descriptor);
                }
                //QApplication::processEvents();
            }
        }
    }

    //build the result list
    return descriptorsCache;
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
            QString name = f.fileName();
            int indexOfPoint = name.lastIndexOf(".");
            if(indexOfPoint > 0){
                name = name.left(indexOfPoint);
            }
            return new Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
        }
        return nullptr;
    }
    catch(...){
        qCritical() << "não carregou " << f.absoluteFilePath();
    }
    return nullptr;
}

void PluginFinder::addPathToScan(std::string path){
    scanPaths.push_back(path);
}
