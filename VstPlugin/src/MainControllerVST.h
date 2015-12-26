#ifndef MAINCONTROLLERVST_H
#define MAINCONTROLLERVST_H

#include "MainController.h"
#include "NinjamController.h"
#include "audio/core/PluginDescriptor.h"
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class NinjamControllerVST : public Controller::NinjamController
{
public:
    explicit NinjamControllerVST(Controller::MainController *c);
    inline bool isWaitingForHostSync() const
    {
        return waitingForHostSync;
    }

    void syncWithHost();
    void waitForHostSync();
    void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate);

private:
    bool waitingForHostSync;
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class JamtabaPlugin;

class MainControllerVST : public Controller::MainController
{
public:
    MainControllerVST(Persistence::Settings settings, JamtabaPlugin *plugin);
    ~MainControllerVST();

    inline bool isRunningAsVstPlugin() const
    {
        return true;
    }

    QString getUserEnvironmentString() const;

    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    Controller::NinjamController *createNinjamController(MainController *c);

    void setCSS(QString css);

    inline Vst::PluginFinder *createPluginFinder()
    {
        return nullptr;
    }

    inline Audio::Plugin *createPluginInstance(const Audio::PluginDescriptor &descriptor)
    {
        Q_UNUSED(descriptor);
        return nullptr;// vst plugin can't load other plugins
    }

    void setSampleRate(int newSampleRate);

    inline int getSampleRate() const
    {
        return sampleRate;
    }

    Midi::MidiDriver *createMidiDriver();

    inline void exit()
    {
    }

    inline void addDefaultPluginsScanPath()
    {
    }

    inline void scanPlugins(bool scanOnlyNewPlugins)
    {
        Q_UNUSED(scanOnlyNewPlugins)
    }

    int getHostBpm() const;

    QString getHostName() const;

    void resizePluginEditor(int newWidth, int newHeight);
private:
    int sampleRate;
    JamtabaPlugin *plugin;
};

#endif // MAINCONTROLLERVST_H
