#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "JamTabaVersion.h"
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

    virtual OSStatus			ProcessBufferLists(
                                                   AudioUnitRenderActionFlags &	ioActionFlags,
                                                   const AudioBufferList &			inBuffer,
                                                   AudioBufferList &				outBuffer,
                                                   UInt32							inFramesToProcess );

};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)


JamTaba::JamTaba(AudioUnit component)
	: AUEffectBase(component)
{
	
}

OSStatus JamTaba::ProcessBufferLists(AudioUnitRenderActionFlags &	ioActionFlags,
                                               const AudioBufferList &			inBuffer,
                                               AudioBufferList &				outBuffer,
                                               UInt32							inFramesToProcess )
{
    if (inBuffer.mNumberBuffers == 2) {
        const float *srcBufferL = (Float32 *)inBuffer.mBuffers[0].mData;
        const float *srcBufferR = (Float32 *)inBuffer.mBuffers[1].mData;
        float *destBufferL = (Float32 *)outBuffer.mBuffers[0].mData;
        float *destBufferR = (Float32 *)outBuffer.mBuffers[1].mData;
        
        for(UInt32 frame = 0; frame < inFramesToProcess; ++frame) {
            destBufferL[frame] = srcBufferL[frame] * 0.1;
            destBufferR[frame] = srcBufferR[frame] * 0.1;
        }
    }

    return noErr;
}



OSStatus JamTaba::Initialize()
{
	OSStatus result = AUEffectBase::Initialize();
	
	return result;
}


OSStatus JamTaba::GetPropertyInfo (	AudioUnitPropertyID				inID,
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

        }
	}
	
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

OSStatus JamTaba::GetProperty (	AudioUnitPropertyID 		inID,
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
				CFBundleRef bundle = CFBundleGetBundleWithIdentifier( CFSTR("com.jamtaba.audiounit.JamTaba") );
				
				if (bundle == NULL)
                    return fnfErr;
                
				CFURLRef bundleURL = CFBundleCopyResourceURL(bundle,
                    CFSTR("CocoaJamTabaView"),	// this is the name of the cocoa bundle
                    CFSTR("bundle"),			// this is the extension of the cocoa bundle
                    NULL);
                
                if (bundleURL == NULL)
                    return fnfErr;
                
				CFStringRef className = CFSTR("DEMOFilter_ViewFactory");// name of the main class
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, { className } };
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
		}
	}
	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}