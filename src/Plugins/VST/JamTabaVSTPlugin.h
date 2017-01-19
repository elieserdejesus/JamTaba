#ifndef JAMTABA_VST_PLUGIN_H
#define JAMTABA_VST_PLUGIN_H

#define uniqueIDEffect CCONST('V', 'b', 'P', 'l')
#define uniqueIDInstrument CCONST('V', 'b', 'I', 's')
#define VST_EVENT_BUFFER_SIZE 1000
#define DEFAULT_INPUTS 2
#define DEFAULT_OUTPUTS 1

#include "JamTabaPlugin.h" // base plugin class

#include "audioeffectx.h"
#include "aeffectx.h"

AudioEffect *createEffectInstance(audioMasterCallback audioMaster);

// ++++++++++++++++++++++++++++++++

class JamTabaVSTPlugin : public JamTabaPlugin, public AudioEffectX
{
public:
    JamTabaVSTPlugin (audioMasterCallback audioMaster);
    ~JamTabaVSTPlugin();

    //void initialize();    // called first time editor is opened
    VstInt32 canDo(char *text);
    void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);

    VstInt32 fxIdle();
    bool needIdle();

    QString getHostName() override;

    bool getEffectName(char *name);
    bool getVendorString(char *text);
    bool getProductString(char *text);
    VstInt32 getVendorVersion();

    VstInt32 getNumMidiInputChannels();
    VstInt32 getNumMidiOutputChannels();

    void open();
    void close() override;
    void setSampleRate(float sampleRate) override;
    float getSampleRate() const override;

    inline VstPlugCategory getPlugCategory();

    void suspend();
    void resume();

    int getHostBpm() const override;

protected:
    char programName[kVstMaxProgNameLen + 1];
    VstEvents *listEvnts;

    VstTimeInfo *timeInfo;

    qint32 getStartPositionForHostSync() const override;
    bool hostIsPlaying() const override;

    MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const override;
};


/** ----------------------- Inline implementations --------------------------- */

VstPlugCategory JamTabaVSTPlugin::getPlugCategory()
{
    return kPlugCategEffect;
}

#endif
