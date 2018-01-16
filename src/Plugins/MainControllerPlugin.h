#ifndef MAINCONTROLLERPLUGIN_H
#define MAINCONTROLLERPLUGIN_H

#include "MainController.h"
#include "NinjamController.h"
#include "audio/core/PluginDescriptor.h"
#include "NinjamControllerPlugin.h"

class JamTabaPlugin;

class MainControllerPlugin : public controller::MainController
{

    friend class NinjamControllerPlugin;

public:
    MainControllerPlugin(const persistence::Settings &settings, JamTabaPlugin *plugin);
    virtual ~MainControllerPlugin();

    QString getUserEnvironmentString() const;

    audio::AudioDriver *createAudioDriver(const persistence::Settings &settings);

    NinjamControllerPlugin *createNinjamController() override;

    inline NinjamControllerPlugin *getNinjamController() const override
    {
        return dynamic_cast<NinjamControllerPlugin *>(ninjamController.data());
    }

    void setCSS(const QString &css) override;

    int addInputTrackNode(audio::LocalInputNode *inputTrackNode) override;

    void setSampleRate(int newSampleRate) override;

    float getSampleRate() const override;

    midi::MidiDriver *createMidiDriver();

    inline void exit() // TODO remove this ??
    {
    }

    int getHostBpm() const;

    QString getHostName() const;

    virtual void resizePluginEditor(int newWidth, int newHeight) = 0;

    persistence::Preset loadPreset(const QString &name) override;

    inline std::vector<midi::MidiMessage> pullMidiMessagesFromPlugins() override
    {
        return std::vector<midi::MidiMessage>(); // empty buffer
    }

protected:
    inline std::vector<midi::MidiMessage> pullMidiMessagesFromDevices() override
    {
        return std::vector<midi::MidiMessage>(); // empty buffer
    }

    JamTabaPlugin *plugin;

};

#endif // MAINCONTROLLERPLUGIN_H
