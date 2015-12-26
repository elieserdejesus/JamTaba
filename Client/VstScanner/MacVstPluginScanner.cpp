#include "VstPluginScanner.h"
#include "audio/core/PluginDescriptor.h"
#include <QFileInfo>
#include <QFile>
#include <QLibrary>
#include <QDebug>
#include "audio/vst/VstLoader.h"
#include "audio/vst/VstHost.h"

Audio::PluginDescriptor VstPluginScanner::getPluginDescriptor(QFileInfo f)
{
    try{
        if (!isVstPluginFile(f.absoluteFilePath()))
            return Audio::PluginDescriptor();// invalid descriptor
        AEffect *effect = Vst::VstLoader::load(f.absoluteFilePath(), Vst::Host::getInstance());
        if (effect) {
            QString name = Audio::PluginDescriptor::getPluginNameFromPath(f.absoluteFilePath());
            Vst::VstLoader::unload(effect);// delete the AEffect instance
            QLibrary lib(f.absoluteFilePath());
            lib.unload();// try unload the shared lib
            return Audio::PluginDescriptor(name, "VST", f.absoluteFilePath());
        }
    }
    catch (...) {
        qCritical() << "Error loading " << f.absoluteFilePath();
    }
    return Audio::PluginDescriptor();// invalid descriptor
}

bool VstPluginScanner::isVstPluginFile(QString path)
{
    QFileInfo file(path);
    bool testResult = file.isBundle() && file.absoluteFilePath().endsWith(".vst");
    return testResult;
}
