#ifndef VST_PLUGIN_H
#define VST_PLUGIN_H

#include "audio/core/Plugins.h"
#include "aeffectx.h"
#include <QMap>
#include <QLibrary>

#include <memory>
#include <vector>

#define MAX_MIDI_EVENTS 64 // in my tests playing piano I can genenerate just 3 messages per block (256 samples) at maximum

struct VstEvents;

namespace vst {

class VstHost;

typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host); // Plugin's entry point

class VstPlugin : public audio::Plugin
{
public:
    explicit VstPlugin(vst::VstHost *host, const QString &pluginPath);
    ~VstPlugin();

    void process(const audio::SamplesBuffer &vstInputArray, audio::SamplesBuffer &outBuffer, std::vector<midi::MidiMessage> &midiBuffer) override;
    void openEditor(const QPoint &centerOfScreen) override;

    void closeEditor() override;

    bool load(const QString &path);

    inline QString getPath() const override
    {
        return path;
    }

    QByteArray getSerializedData() const override;

    void restoreFromSerializedData(const QByteArray &dataToRestore) override;

    void start() override;

    void updateGui() override;

    void setSampleRate(int newSampleRate) override;

    void setBypass(bool state) override;

    static QDialog *getPluginEditorWindow(const QString &pluginName);

    bool isVirtualInstrument() const override;

    bool canGenerateMidiMessages() const override;

    inline quint32 getPluginID() const { return effect->resvd1; }

protected:
    void unload();
    void resume() override;
    void suspend() override;

private:
    bool initPlugin();

    AEffect *effect;

    std::unique_ptr<audio::SamplesBuffer> internalOutputBuffer;
    std::unique_ptr<audio::SamplesBuffer> internalInputBuffer;

    vst::VstHost *host;

    bool wantMidi;

    QString path;

    bool started;
    bool turnedOn;

    bool loaded;

    void fillVstEventsList(const std::vector<midi::MidiMessage> &midiBuffer);

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
