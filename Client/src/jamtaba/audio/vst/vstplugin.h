#pragma once

#include <QDialog>
#include "../core/plugins.h"
#include "aeffect.h"

namespace Vst {

class VstPlugin : public Audio::Plugin
{
public:
    VstPlugin(QString name, QString file, AEffect* effect);
    ~VstPlugin();
    virtual void process(Audio::SamplesBuffer &buffer);
    virtual void openEditor(Audio::PluginWindow *, QPoint p);
private:
    AEffect* effect;
};

Vst::VstPlugin* load(QString pluginPath);

}
