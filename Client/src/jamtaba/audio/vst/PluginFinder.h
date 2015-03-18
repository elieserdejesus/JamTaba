#pragma once
#include <memory>
#include <vector>
#include <string>
#include <QFileInfo>
#include <QObject>
#include <QThread>

namespace Audio {
    class PluginDescriptor;
}

namespace Vst {

class VstHost;

class ScanThread;

class PluginFinder : public QThread
{
    Q_OBJECT

public:
    PluginFinder();
    ~PluginFinder();
    void addPathToScan(std::string path);
    void scan(Vst::VstHost *host);
private:
    void run();
signals:
    void scanStarted();
    void scanFinished();
    void vstPluginFounded(Audio::PluginDescriptor* pluginDescriptor);
private:
    std::vector<std::string> scanPaths;
    Audio::PluginDescriptor* getPluginDescriptor(QFileInfo f, Vst::VstHost* host);
    Vst::VstHost* host;
};

}

