#include "PluginFinder.h"

using namespace Vst;

PluginFinder::PluginFinder()
    :cancelRequested(false){

}

void PluginFinder::cancel(){
    this->cancelRequested = true;
}

void PluginFinder::clearScanFolders(){
    scanFolders.clear();
}

void PluginFinder::addFolderToScan(QString path){
    if(!scanFolders.contains(path)){
        scanFolders.append(path);
    }
}
