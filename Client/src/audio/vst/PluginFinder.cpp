#include "PluginFinder.h"
#include "audio/core/PluginDescriptor.h"

using namespace Vst;

PluginFinder::PluginFinder()
{

}

void PluginFinder::setFoldersToScan(QStringList folders){
    scanFolders.clear();
    scanFolders.append(folders);
}
