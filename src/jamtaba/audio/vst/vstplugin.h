#pragma once

#include "../core/plugins.h"
#include "aeffect.h"
#include <QLibrary>
//#include "../audio/core/SamplesBuffer.h"

namespace Vst {

class VstHost;

class VstPlugin : public Audio::Plugin
{
public:
    explicit VstPlugin(Vst::VstHost* host);
    ~VstPlugin();
    void start(int sampleRate, int bufferSize);
    void resume();
    void suspend();
    virtual void process(Audio::SamplesBuffer &buffer);
    virtual void openEditor(Audio::PluginWindow *, QPoint p);
    bool load(Vst::VstHost* host, QString path);
protected:
    void unload();
    //void processMidiEvents(Midi::MidiBuffer& midiIn);
private:
    bool initPlugin();
    AEffect* effect;
    Audio::SamplesBuffer* internalBuffer;
    QLibrary pluginLib;
    Vst::VstHost* host;
    bool wantMidi;
};


}
