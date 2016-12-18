
#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "JamTabaVersion.h"
#include "JamTaba.h"
#include <math.h>


class JamTaba : public AUEffectBase
{
public:
	JamTaba(AudioUnit component);

	virtual OSStatus			Version() { return kJamTabaVersion; }

	virtual OSStatus			Initialize();

	// for custom property
	virtual OSStatus			GetPropertyInfo(	AudioUnitPropertyID		inID,
													AudioUnitScope			inScope,
													AudioUnitElement		inElement,
													UInt32 &				outDataSize,
													Boolean	&				outWritable );

	virtual OSStatus			GetProperty(		AudioUnitPropertyID 	inID,
													AudioUnitScope 			inScope,
													AudioUnitElement 		inElement,
													void 					* outData );


};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)


enum
{
	kFilterParam_CutoffFrequency = 0,
	kFilterParam_Resonance = 1
};


static CFStringRef kCutoffFreq_Name = CFSTR("cutoff frequency");
static CFStringRef kResonance_Name = CFSTR("resonance");


const float kDefaultCutoff = 1000.0;
const float kDefaultResonance = 0;



// Factory presets
static const int kPreset_One = 0;
static const int kPreset_Two = 1;
static const int kNumberPresets = 2;

static AUPreset kPresets[kNumberPresets] = 
    {
        { kPreset_One, CFSTR("Preset One") },		
        { kPreset_Two, CFSTR("Preset Two") }		
	};
	


JamTaba::JamTaba(AudioUnit component)	: AUEffectBase(component)
{
	// all the parameters must be set to their initial values here
	//
	// these calls have the effect both of defining the parameters for the first time
	// and assigning their initial values
	//
	SetParameter(kFilterParam_CutoffFrequency, kDefaultCutoff );
	SetParameter(kFilterParam_Resonance, kDefaultResonance );

	// kFilterParam_CutoffFrequency max value depends on sample-rate
	SetParamHasSampleRateDependency(true );
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::Initialize
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			JamTaba::Initialize()
{
	OSStatus result = AUEffectBase::Initialize();
	
	if(result == noErr )
	{
		// in case the AU was un-initialized and parameters were changed, the view can now
		// be made aware it needs to update the frequency response curve
		PropertyChanged(kAudioUnitCustomProperty_FilterFrequencyResponse, kAudioUnitScope_Global, 0 );
	}
	
	return result;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::GetPropertyInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus			JamTaba::GetPropertyInfo (	AudioUnitPropertyID				inID,
												AudioUnitScope					inScope,
												AudioUnitElement				inElement,
												UInt32 &						outDataSize,
												Boolean &						outWritable)
{
	if (inScope == kAudioUnitScope_Global)
	{
		switch (inID)
		{
			case kAudioUnitProperty_CocoaUI:
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;

			case kAudioUnitCustomProperty_FilterFrequencyResponse:	// our custom property
				if(inScope != kAudioUnitScope_Global ) return kAudioUnitErr_InvalidScope;
				outDataSize = kNumberOfResponseFrequencies * sizeof(FrequencyResponse);
				outWritable = false;
				return noErr;
		}
	}
	
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}



OSStatus			JamTaba::GetProperty (	AudioUnitPropertyID 		inID,
											AudioUnitScope 				inScope,
											AudioUnitElement			inElement,
											void *						outData)
{
	if (inScope == kAudioUnitScope_Global)
	{
		switch (inID)
		{
			// This property allows the host application to find the UI associated with this
			// AudioUnit
			//
			case kAudioUnitProperty_CocoaUI:
			{
				// Look for a resource in the main bundle by name and type.
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("com.jamtaba") );
				
				if (bundle == NULL) return fnfErr;
                
				CFURLRef bundleURL = CFBundleCopyResourceURL( bundle, 
                    CFSTR("CocoaFilterView"),	// this is the name of the cocoa bundle as specified in the CocoaViewFactory.plist
                    CFSTR("bundle"),			// this is the extension of the cocoa bundle
                    NULL);
                
                if (bundleURL == NULL) return fnfErr;
                
				CFStringRef className = CFSTR("DEMOFilter_ViewFactory");	// name of the main class that implements the AUCocoaUIBase protocol
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, { className } };
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
		}
	}
	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}



