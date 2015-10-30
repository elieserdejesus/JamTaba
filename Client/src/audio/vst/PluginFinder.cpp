#include "PluginFinder.h"

using namespace Vst;

void PluginFinder::clearScanFolders(){
    scanFolders.clear();
}

void PluginFinder::addFolderToScan(QString path){
    if(!scanFolders.contains(path)){
        scanFolders.append(path);
    }
}
