#include "PluginFinder.h"

using namespace Vst;

void PluginFinder::clearScanPaths(){
    scanPaths.clear();
}

void PluginFinder::addPathToScan(QString path){
    if(!scanPaths.contains(path)){
        scanPaths.append(path);
    }
}
