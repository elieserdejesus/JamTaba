#ifndef VST_PLUGIN_H
#define VST_PLUGIN_H

#include "audio/core/Plugins.h"
#include "aeffectx.h"
#include <QMap>
#include <QLibrary>

#define MAX_MIDI_EVENTS 40 // in my tests playing piano I can genenerate just 3 messages per block (256 samples) at maximum

namespace Vst {
class Host;
class VstEvents;

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

class VstPlugin : public Audio::Plugin
{
public:
    explicit VstPlugin(Vst::Host *host);
    ~VstPlugin();

    void process(const Audio::SamplesBuffer &vstInputArray, Audio::SamplesBuffer &outBuffer,
                         const Midi::MidiBuffer &midiBuffer) override;
    void openEditor(const QPoint &centerOfScreen) override;
    void closeEditor() override;
    bool load(QString path);
    inline QString getPath() const
    {
        return path;
    }

    QByteArray getSerializedData() const override;
    void restoreFromSerializedData(const QByteArray &dataToRestore) override;
    void start();
    void updateGui();
    void setSampleRate(int newSampleRate);
    void setBypass(bool state);
    static QDialog *getPluginEditorWindow(QString pluginName);
    bool isVirtualInstrument() const;
protected:
    void unload();

    void resume();
    void suspend();
private:
    bool initPlugin();
    AEffect *effect;
    Audio::SamplesBuffer *internalOutputBuffer;
    Audio::SamplesBuffer *internalInputBuffer;
    QLibrary pluginLib;
    Vst::Host *host;
    bool wantMidi;
    QString path;

    bool started;
    bool turnedOn;

    bool loaded;

    QMutex editorMutex;

    float **vstOutputArray;
    float **vstInputArray;

    // VstEvents* vstEvents;
    void fillVstEventsList(const Midi::MidiBuffer &midiBuffer);

    template<int N>
    struct VSTEventBlock
    {
        VstInt32 numEvents;
        VstIntPtr reserved;
        VstEvent *events[N];
    };

    VSTEventBlock<MAX_MIDI_EVENTS> vstMidiEvents;

    static QMap<QString, QDialog *> editorsWindows;
};
}

#endif
