#pragma once

#include "../core/plugins.h"
#include "aeffectx.h"
#include <QLibrary>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(vst)

#define MAX_MIDI_EVENTS 40 //in my tests playing piano I can genenerate just 3 messages per block (256 samples) at maximum

namespace Vst {

class VstHost;
class VstEvents;

class VstPlugin : public Audio::Plugin
{
public:

    explicit VstPlugin(Vst::VstHost* host);
    ~VstPlugin();
    void start();
    void resume();
    void suspend();
    virtual void process( Audio::SamplesBuffer &audioBuffer, const Midi::MidiBuffer& midiBuffer);
    virtual void openEditor(QPoint centerOfScreen);
    bool load(Vst::VstHost* host, QString path);
    inline QString getPath() const{return path;}
    virtual QByteArray getSerializedData() const;
    virtual void restoreFromSerializedData(QByteArray dataToRestore);
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
    QString path;

    bool started;

    //VstEvents* vstEvents;
    void fillVstEventsList(const Midi::MidiBuffer& midiBuffer);

    template <int N>
    struct VSTEventBlock
    {
        VstInt32 numEvents;
        VstIntPtr reserved;
        VstEvent* events[N];
    };

    VSTEventBlock<MAX_MIDI_EVENTS> vstMidiEvents;
};


}
