#pragma once
#include <memory>
#include <vector>
#include <string>
#include <QFileInfo>

namespace Audio {
    class PluginDescriptor;
}

namespace Vst {

class VstHost;

class PluginFinder
{
public:
    PluginFinder();
    ~PluginFinder();
    void addPathToScan(std::string path);
    std::vector<Audio::PluginDescriptor*> scan(Vst::VstHost *host);
private:
    std::vector<Audio::PluginDescriptor*> descriptorsCache;
    std::vector<std::string> scanPaths;
    Audio::PluginDescriptor* getPluginDescriptor(QFileInfo f, Vst::VstHost* host);
};

}

