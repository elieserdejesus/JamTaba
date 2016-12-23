#ifndef JamTaba_JamTaba_h
#define JamTaba_JamTaba_h

#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "JamTabaVersion.h"
#include <math.h>

enum
{
    kAudioUnitCustomProperty_JamTabaListener = 65537,

};




class JamTabaListener
{
public:
    virtual void audioCallback(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess) = 0;
};




class JamTaba : public AUEffectBase
{
public:
	JamTaba(AudioUnit component);
    
	inline OSStatus Version() override {
        return kJamTabaVersion;
    }
	
    OSStatus Initialize() override;
	
    OSStatus GetPropertyInfo(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32 &outDataSize, Boolean	&outWritable) override;
    
	OSStatus GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void	*outData) override;
    
    OSStatus ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags, const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess) override;
    
    OSStatus SetProperty(		AudioUnitPropertyID inID,
                                                  AudioUnitScope 		inScope,
                                                  AudioUnitElement 	inElement,
                                                  const void *			inData,
                                                  UInt32 inDataSize)         override;
    
   
private:
    JamTabaListener *listener;
    
};


#endif
