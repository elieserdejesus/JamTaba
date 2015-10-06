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

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

class VstPlugin : public Audio::Plugin{
public:
    explicit VstPlugin(Vst::VstHost* host);
    ~VstPlugin();

    virtual void process(const Audio::SamplesBuffer& vstInputArray, Audio::SamplesBuffer &outBuffer, const Midi::MidiBuffer& midiBuffer);
    virtual void openEditor(QPoint centerOfScreen);
    virtual void closeEditor();
    bool load(QString path);
    inline QString getPath() const{return path;}
    virtual QByteArray getSerializedData() const;
    virtual void restoreFromSerializedData(QByteArray dataToRestore);
    void start();
    void updateGui();
    void setSampleRate(int newSampleRate);
protected:
    void unload();

    void resume();
    void suspend();
    //void processMidiEvents(Midi::MidiBuffer& midiIn);
private:
    bool initPlugin();
    AEffect* effect;
    Audio::SamplesBuffer* internalOutputBuffer;
    Audio::SamplesBuffer* internalInputBuffer;
    QLibrary pluginLib;
    Vst::VstHost* host;
    bool wantMidi;
    //bool canProcessReplacing;
    QString path;

    bool started;
    bool turnedOn;

    bool loaded;

    QMutex editorMutex;

    float** vstOutputArray;
    float** vstInputArray;


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
