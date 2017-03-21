#ifndef JamTaba_JamTaba_h
#define JamTaba_JamTaba_h

#include "AUBase.h"
#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "JamTabaVersion.h"
#include <math.h>

enum
{
    kJamTabaSetListener         = 65537,
    kJamTabaGetHostState
};


struct AUHostState
{
	double samplePos;       // current Position in audio samples (always valid)
	double sampleRate;      // current Sample Rate in Herz (always valid)
	double ppqPos;          // Musical Position, in Quarter Note (1.0 equals 1 Quarter Note)
	double tempo;           // current Tempo in BPM (Beats Per Minute)
	double barStartPos;     // last Bar Start Position, in Quarter Note
	int timeSigNumerator;   // Time Signature Numerator (e.g. 3 for 3/4)
	int timeSigDenominator; // Time Signature Denominator (e.g. 4 for 3/4)
    Boolean playing;        // host is playing
};


/** This interface is the communication pointbetween this Audio Unit and the cocoa View (see JamTabaAUPlugin.mm) */

class JamTabaAudioUnitListener
{
public:
    virtual void process(Float32 **inputs, Float32 ** outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess, const AUHostState &hostState) = 0;
    
    virtual void cleanUp() = 0;
};



class JamTaba : public AUEffectBase
{
    
public:
    
	JamTaba(AudioUnit component);
    
	inline OSStatus Version() override {
        return kJamTabaVersion;
    }
    
    void Cleanup() override;
	
    OSStatus GetPropertyInfo(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32 &outDataSize, Boolean	&outWritable) override;
    
	OSStatus GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void	*outData) override;
    
    OSStatus SetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, const void *inData, UInt32 inDataSize) override;
   
	bool StreamFormatWritable (AudioUnitScope scope, AudioUnitElement element) override;
    
    OSStatus Render(AudioUnitRenderActionFlags &ioActionFlags, const AudioTimeStamp &inTimeStamp, UInt32 nFrames) override;
    
private:
    
    JamTabaAudioUnitListener *listener;
    AUHostState hostState;
    
    void updateHostState();
    
};


#endif
