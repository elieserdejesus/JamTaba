#ifndef JamTaba_JamTaba_h
#define JamTaba_JamTaba_h

#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "JamTabaVersion.h"
#include <math.h>
#include "JamTabaPlugin.h" // plugins (AU and VST) base class
#include "JamTaba.h"

class JamTaba : public AUEffectBase, public JamTabaPlugin
{
public:
	JamTaba(AudioUnit component);
    
	virtual OSStatus			Version() { return kJamTabaVersion; }
	virtual OSStatus			Initialize();
	virtual OSStatus			GetPropertyInfo(AudioUnitPropertyID	inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32 &outDataSize, Boolean	&outWritable);
    
	virtual OSStatus GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void	*outData);
    
    virtual OSStatus ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags, const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess);
    
    QString getHostName() override;
    
    float getSampleRate() const override;
    
    int getHostBpm() const override;
    
    bool hostIsPlaying() const override;
    
    qint32 getStartPositionForHostSync() const override;
    
    MainControllerPlugin *createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const override;
    
    inline static JamTaba *getInstance() { return instance; }
    
private:
    static JamTaba *instance;
    
};


#endif
