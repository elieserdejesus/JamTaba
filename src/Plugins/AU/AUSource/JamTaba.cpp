#include "JamTaba.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)


JamTaba::JamTaba(AudioUnit component)
	: AUEffectBase(component),
      listener(nullptr)
{
    
}


OSStatus JamTaba::SetProperty(AudioUnitPropertyID inID,
                                AudioUnitScope 		inScope,
                                AudioUnitElement 	inElement,
                                const void *			inData,
                                                UInt32 inDataSize)
{
    if (inScope == kAudioUnitScope_Global && inID == kJamTabaSetListener) {
        this->listener = (JamTabaAudioUnitListener*)inData;
        return noErr;
    }
    
    return AUEffectBase::SetProperty (inID, inScope, inElement, inData, inDataSize);
}

void JamTaba::Cleanup()
{
    AUEffectBase::Cleanup();
    
    if (listener)
        listener->cleanUp();
}

OSStatus JamTaba::ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags, const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
{
    if (listener) {
        listener->process(inBuffer, outBuffer, inFramesToProcess);
    }

    return noErr;
}


OSStatus JamTaba::GetPropertyInfo (AudioUnitPropertyID	inID, AudioUnitScope inScope, AudioUnitElement inElement, UInt32 &outDataSize, Boolean &outWritable)
{
	if (inScope == kAudioUnitScope_Global)
	{
		switch (inID)
		{
			case kAudioUnitProperty_CocoaUI:
				outWritable = false;
				outDataSize = sizeof (AudioUnitCocoaViewInfo);
				return noErr;
                
            case kJamTabaGetHostBPM:
                outWritable = true;
                outDataSize = sizeof(int *);
                return noErr;
            
            case kJamTabaGetHostSampleRate:
                outWritable = true;
                outDataSize = sizeof(int *);
                return noErr;
                
        }
	}
	
	return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}


OSStatus JamTaba::GetProperty(AudioUnitPropertyID inID, AudioUnitScope inScope, AudioUnitElement inElement, void *outData)
{
    
    //qDebug() << "inID: " << inID << " inScope: " << inScope;
	if (inScope == kAudioUnitScope_Global)
	{
		switch (inID)
		{
			// This property allows the host application to find the UI associated with this AudioUnit
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
                
				CFStringRef className = CFSTR("JamTaba_ViewFactory");// name of the main class
				AudioUnitCocoaViewInfo cocoaInfo = { bundleURL, { className } };
				*((AudioUnitCocoaViewInfo *)outData) = cocoaInfo;
				
				return noErr;
			}
                
            case kJamTabaGetHostBPM:
            {
                Float64		bpm;
                OSStatus	err	= CallHostBeatAndTempo(NULL, &bpm);
                
                if (err == noErr) {
                    *((int*)outData) = (int)bpm;
                    
                    return err;
                }
            }
                
            case kJamTabaGetHostSampleRate:
            {
                 *((int*)outData) = (int)GetSampleRate();
                return noErr;
            }
  		}
	}
	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}