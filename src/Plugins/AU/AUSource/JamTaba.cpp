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

void JamTaba::updateHostState()
{
    Boolean hostIsPlaying;
    Boolean hostTransportChanged;
    Float64 currentSampleInTimeLine;
    Boolean hostIsCycling;
    Float64 cycleStartBeat;
    Float64 cycleEndBeat;
    
    CallHostTransportState(&hostIsPlaying, &hostTransportChanged, &currentSampleInTimeLine, &hostIsCycling, &cycleStartBeat, &cycleEndBeat);
    
    
    UInt32 deltaSampleOffsetToNextBeat;
    Float32 timeSigNumerator;
    UInt32 timeSigDenominator;
    Float64 currentMeasureDownBeat;
    
    CallHostMusicalTimeLocation(&deltaSampleOffsetToNextBeat, &timeSigNumerator, &timeSigDenominator, &currentMeasureDownBeat);
    
    
    Float64 currentBeat;
    Float64 currentTempo;
    CallHostBeatAndTempo(&currentBeat, &currentTempo);
    
    hostState.sampleRate            = GetSampleRate();
    hostState.tempo                 = currentTempo;
    hostState.timeSigDenominator    = timeSigDenominator;
    hostState.timeSigNumerator      = timeSigNumerator;
    hostState.samplePos             = currentSampleInTimeLine;
    hostState.playing               = hostIsPlaying;
    
    
    double dPos = hostState.samplePos / hostState.sampleRate;
    hostState.ppqPos = dPos * hostState.tempo / 60.L;
    
    // bar length in quarter notes
    float barLengthq = (float)(4 * hostState.timeSigNumerator)/hostState.timeSigDenominator;
    int currentBar = std::floor(hostState.ppqPos/barLengthq);
    hostState.barStartPos = barLengthq * currentBar;
}

OSStatus JamTaba::ProcessBufferLists(AudioUnitRenderActionFlags &ioActionFlags, const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
{
    if (listener) {
        updateHostState();
        listener->process(inBuffer, outBuffer, inFramesToProcess, hostState);
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

  		}
	}
	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}