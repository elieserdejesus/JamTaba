#ifndef MAINCONTROLLERVST_H
#define MAINCONTROLLERVST_H

#include "MainController.h"
#include "NinjamController.h"
#include "audio/core/PluginDescriptor.h"
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainControllerVST;

class NinjamControllerVST : public Controller::NinjamController
{
public:
    explicit NinjamControllerVST(MainControllerVST *c);
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

    inline QString getJamtabaFlavor() const override
    {
        return "Vst Plugin";
    }

    QString getUserEnvironmentString() const;

    Audio::AudioDriver *createAudioDriver(const Persistence::Settings &settings);

    Controller::NinjamController *createNinjamController() override;

    void setCSS(QString css);

    int addInputTrackNode(Audio::LocalInputAudioNode *inputTrackNode) override;

    void setSampleRate(int newSampleRate);

    inline int getSampleRate() const override
    {
        return sampleRate;
    }

    Midi::MidiDriver *createMidiDriver();

    inline void exit() //TODO remove this ??
    {
    }

    int getHostBpm() const;

    QString getHostName() const;

    void resizePluginEditor(int newWidth, int newHeight);

protected:
    inline Midi::MidiBuffer pullMidiBuffer() override
    {
        return Midi::MidiBuffer(0);
    }
private:
    int sampleRate;
    JamtabaPlugin *plugin;
};

#endif // MAINCONTROLLERVST_H
