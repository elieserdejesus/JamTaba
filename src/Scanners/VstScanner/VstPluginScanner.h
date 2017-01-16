#ifndef VSTPLUGINSCANNER_H
#define VSTPLUGINSCANNER_H

#include "BaseScanner.h"

class VstPluginScanner : public BaseScanner
{

public:
    VstPluginScanner();

private:
    bool canScan(const QFileInfo &pluginFileInfo) const;

    QStringList foldersToScan;
    QStringList skipList; //contain blackListed and cached plugins

    void initialize(int argc, char *argv[]) override;

    Audio::PluginDescriptor getPluginDescriptor(const QFileInfo &pluginFile);

protected:

    void scan() override;
};

#endif // VSTPLUGINSCANNER_H
