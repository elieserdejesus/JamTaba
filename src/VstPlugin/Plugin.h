#ifndef VST_PLUGIN_H
#define VST_PLUGIN_H

#define uniqueIDEffect CCONST('V', 'b', 'P', 'l')
#define uniqueIDInstrument CCONST('V', 'b', 'I', 's')
#define VST_EVENT_BUFFER_SIZE 1000
#define DEFAULT_INPUTS 2
#define DEFAULT_OUTPUTS 1

#include "MainControllerVST.h"

#include "audioeffectx.h"
#include "aeffectx.h"

AudioEffect *createEffectInstance(audioMasterCallback audioMaster);

// ++++++++++++++++++++++++++++++++

class JamtabaPlugin : public AudioEffectX
{
public:
    static bool pluginIsInitialized();
    JamtabaPlugin (audioMasterCallback audioMaster);
    ~JamtabaPlugin ();
    inline bool isRunning() const
    {
        return running;
    }

    void initialize();    // called first time editor is opened
    VstInt32 canDo(char *text);
    void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);

    VstInt32 fxIdle();
    bool needIdle();

    QString getHostName();

    bool getEffectName(char *name);
    bool getVendorString(char *text);
    bool getProductString(char *text);
    VstInt32 getVendorVersion();

    VstInt32 getNumMidiInputChannels();
    VstInt32 getNumMidiOutputChannels();

    void open();
    void close();
    void setSampleRate(float sampleRate);
    inline int getSampleRate() const
    {
        return sampleRate;
    }

    void suspend();
    void resume();
    inline VstPlugCategory getPlugCategory()
    {
        return kPlugCategEffect;
    }

    inline MainControllerVST *getController()
    {
        return controller.data();
    }

    int getHostBpm() const;

protected:
    char programName[kVstMaxProgNameLen + 1];
    int sampleRate;
    VstEvents *listEvnts;

private:
    QScopedPointer<MainControllerVST> controller;
    bool running;
    Audio::SamplesBuffer inputBuffer;
    Audio::SamplesBuffer outputBuffer;

    VstTimeInfo *timeInfo;
    bool hostWasPlayingInLastAudioCallBack;

    bool hostIsPlaying() const;

    inline bool transportStartDetectedInHost() const
    {
        return hostIsPlaying() && !hostWasPlayingInLastAudioCallBack;
    }

    static bool instanceIsInitialized;

    qint32 getStartPositionForHostSync() const;
};

#endif // VST_H
