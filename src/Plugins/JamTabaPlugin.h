#ifndef JAMTABA_PLUGIN_H
#define JAMTABA_PLUGIN_H

#include "MainControllerPlugin.h"

/** this is the base class for VST and AU plugins */

class JamTabaPlugin
{
public:
    static bool pluginIsInitialized();

    JamTabaPlugin(quint8 inputChannels, quint8 outputChannels);
    virtual ~JamTabaPlugin();
    
    MainControllerPlugin *getController();

    void initialize();    // need be called first time editor is opened

    virtual QString getHostName() = 0;

    virtual void close();
    virtual void setSampleRate(float sampleRate);
    virtual float getSampleRate() const = 0;

    inline bool isRunning() const;

    virtual int getHostBpm() const = 0;

protected:
    //int sampleRate;
    QScopedPointer<MainControllerPlugin> controller;
    bool running;
    Audio::SamplesBuffer inputBuffer;
    Audio::SamplesBuffer outputBuffer;
    bool hostWasPlayingInLastAudioCallBack;

    

    static bool instanceIsInitialized;

    inline bool transportStartDetectedInHost() const;

    virtual bool hostIsPlaying() const = 0;
    
    virtual qint32 getStartPositionForHostSync() const = 0;

    virtual MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const = 0;

};


/** ----------------------- Inline implementations --------------------------- */

bool JamTabaPlugin::isRunning() const
{
    return running;
}

bool JamTabaPlugin::transportStartDetectedInHost() const
{
    return hostIsPlaying() && !hostWasPlayingInLastAudioCallBack;
}

#endif
