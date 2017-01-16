#ifndef VST_PLUGIN_H
#define VST_PLUGIN_H

#include "audio/core/Plugins.h"
#include "aeffectx.h"
#include <QMap>
#include <QLibrary>

#define MAX_MIDI_EVENTS 64 // in my tests playing piano I can genenerate just 3 messages per block (256 samples) at maximum

struct VstEvents;

namespace Vst {

class VstHost;

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

class VstPlugin : public Audio::Plugin
{
public:
    explicit VstPlugin(Vst::VstHost *host, const QString &pluginPath);
    ~VstPlugin();

    void process(const Audio::SamplesBuffer &vstInputArray, Audio::SamplesBuffer &outBuffer,
                         const QList<Midi::MidiMessage> &midiBuffer) override;
    void openEditor(const QPoint &centerOfScreen) override;

    void closeEditor() override;

    bool load(const QString &path);

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

    static QDialog *getPluginEditorWindow(const QString &pluginName);

    bool isVirtualInstrument() const override;

    bool canGenerateMidiMessages() const override;

    inline quint32 getPluginID() const { return effect->resvd1; }

protected:
    void unload();
    void resume();
    void suspend();

private:
    bool initPlugin();

    AEffect *effect;

    Audio::SamplesBuffer *internalOutputBuffer;
    Audio::SamplesBuffer *internalInputBuffer;

    Vst::VstHost *host;

    bool wantMidi;

    QString path;

    bool started;
    bool turnedOn;

    bool loaded;

    float **vstOutputArray;
    float **vstInputArray;

    // VstEvents* vstEvents;
    void fillVstEventsList(const QList<Midi::MidiMessage> &midiBuffer);

    template<int N>
    struct VSTEventBlock
    {
        VstInt32 numEvents;
        VstIntPtr reserved;
        VstEvent *events[N];
    };

    VSTEventBlock<MAX_MIDI_EVENTS> vstMidiEvents;

    static QMap<QString, QDialog *> editorsWindows;

}; // class


} // namespace

#endif
