#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"

using namespace Vst;

PluginFinder::PluginFinder()
{

}

void PluginFinder::clearScanFolders(){
    scanFolders.clear();
}

void PluginFinder::addFolderToScan(QString path){
    if(!scanFolders.contains(path)){
        scanFolders.append(path);
    }
}
