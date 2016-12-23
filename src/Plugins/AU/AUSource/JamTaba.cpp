#include "JamTaba.h"
//#include "MainControllerPlugin.h"



//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)


JamTaba::JamTaba(AudioUnit component)
	: AUEffectBase(component),
      listener(nullptr)
{
	//SetProperty(kAudioUnitCustomProperty_JamTabaPluginIntance, kAudioUnitScope_Global, 0, this, sizeof(JamTaba));
}

OSStatus JamTaba::SetProperty(AudioUnitPropertyID inID,
                                AudioUnitScope 		inScope,
                                AudioUnitElement 	inElement,
                                const void *			inData,
                                                UInt32 inDataSize)
{
    if (inScope == kAudioUnitScope_Global && inID == kAudioUnitCustomProperty_JamTabaListener) {
        this->listener = (JamTabaListener*)inData;
        printf("\n\nlistener settado para %d\n\n", this->listener);
        return noErr;
    }
    
    return AUEffectBase::SetProperty (inID, inScope, inElement, inData, inDataSize);
}



OSStatus JamTaba::ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags, const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
{
//    if (inBuffer.mNumberBuffers == 2) {
//        const float *srcBufferL = (Float32 *)inBuffer.mBuffers[0].mData;
//        const float *srcBufferR = (Float32 *)inBuffer.mBuffers[1].mData;
//        float *destBufferL = (Float32 *)outBuffer.mBuffers[0].mData;
//        float *destBufferR = (Float32 *)outBuffer.mBuffers[1].mData;
//        
//        for(UInt32 frame = 0; frame < inFramesToProcess; ++frame) {
//            destBufferL[frame] = srcBufferL[frame] * 0.1;
//            destBufferR[frame] = srcBufferR[frame] * 0.1;
//        }
//    }
    
    if (listener) {
        listener->audioCallback(inBuffer, outBuffer, inFramesToProcess);
    }

    return noErr;
}


OSStatus JamTaba::Initialize()
{
	OSStatus result = AUEffectBase::Initialize();
	return result;
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
            
//            case kAudioUnitCustomProperty_JamTabaPluginIntance:	// our custom property
//				
//                if(inScope != kAudioUnitScope_Global )
//                    return kAudioUnitErr_InvalidScope;
//				
//                outDataSize = sizeof(void *);
//				outWritable = false;
//				return noErr;

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
                
//     		case kAudioUnitCustomProperty_JamTabaPluginIntance:
//			{
//				if(inScope != kAudioUnitScope_Global)
//                    return kAudioUnitErr_InvalidScope;
//                
//                ((void **)outData)[0] = (void *)this;
//                
//				return noErr;
//			}
		}
	}
	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}