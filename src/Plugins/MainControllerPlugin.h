#ifndef MAINCONTROLLERPLUGIN_H
#define MAINCONTROLLERPLUGIN_H

#include "MainController.h"
#include "NinjamController.h"
#include "audio/core/PluginDescriptor.h"
#include "NinjamControllerPlugin.h"
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class JamTabaPlugin;

class MainControllerPlugin : public Controller::MainController
{

    friend class NinjamControllerPlugin;

public:
    MainControllerPlugin(const Persistence::Settings &settings, JamTabaPlugin *plugin);
    virtual ~MainControllerPlugin();

    QString getUserEnvironmentString() const;

    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    NinjamControllerPlugin *createNinjamController() override;

    inline NinjamControllerPlugin *getNinjamController() const override
    {
        return dynamic_cast<NinjamControllerPlugin *>(ninjamController.data());
    }

    void setCSS(const QString &css) override;

    int addInputTrackNode(Audio::LocalInputNode *inputTrackNode) override;

    void setSampleRate(int newSampleRate) override;

    float getSampleRate() const override;

    Midi::MidiDriver *createMidiDriver();

    inline void exit() //TODO remove this ??
    {
    }

    int getHostBpm() const;

    QString getHostName() const;

    virtual void resizePluginEditor(int newWidth, int newHeight) = 0;

    Persistence::Preset loadPreset(const QString &name) override;

    inline Midi::MidiMessageBuffer pullMidiMessagesFromPlugins() override
    {
        static Midi::MidiMessageBuffer emptyBuffer(0);
        return emptyBuffer;
    }

protected:
    inline Midi::MidiMessageBuffer pullMidiMessagesFromDevices() override
    {
        static Midi::MidiMessageBuffer emptyBuffer(0);
        return emptyBuffer;
    }

    JamTabaPlugin *plugin;

};

#endif // MAINCONTROLLERPLUGIN_H
