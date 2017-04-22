#include "JamTaba.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)

JamTabaAUInterface *JamTaba::jamTabaInstance = nullptr;
bool JamTaba::recentlyCreated = false;


JamTaba::JamTaba(AudioUnit component)
	: AUEffectBase(component, true)//2, 1),
{
    JamTaba::recentlyCreated = true;
}

JamTaba::~JamTaba()
{
    // wait 2 seconds to see if the AU will be created. When buffer size is changed in Logic (for example) the AU is deleted and recreated.
    
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        
        if (!JamTaba::recentlyCreated && JamTaba::jamTabaInstance) {
            delete jamTabaInstance;
            jamTabaInstance = nullptr;
        }
    
    });

    JamTaba::recentlyCreated = false;
    
}

OSStatus JamTaba::SetProperty(AudioUnitPropertyID inID,
                                AudioUnitScope 		inScope,
                                AudioUnitElement 	inElement,
                                const void *			inData,
                                                UInt32 inDataSize)
{
    if (inScope == kAudioUnitScope_Global && inID == kJamTabaSetInstance) {
        JamTaba::jamTabaInstance = (JamTabaAUInterface *)inData;
        return noErr;
    }
    
    return AUEffectBase::SetProperty (inID, inScope, inElement, inData, inDataSize);
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
    
    
    hostState.sampleRate            = GetOutput(0)->GetStreamFormat().mSampleRate;
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

OSStatus JamTaba::Render(AudioUnitRenderActionFlags &ioActionFlags, const AudioTimeStamp &timeStamp, UInt32 framesToProcess)
{
    if (!IsInitialized())
        return noErr;
    
	bool firstInputChannelIsActivated = HasInput(0);
    bool secondInputChannelIsActivated = HasInput(1);
    
    if (!firstInputChannelIsActivated) {
        return kAudioUnitErr_NoConnection;
    }
    
    AUInputElement *firstInputBus = GetInput(0);
    AUOutputElement *outputBus = GetOutput(0);
    
    
	OSStatus status = firstInputBus->PullInput(ioActionFlags, timeStamp, 0, framesToProcess);
    if (status != noErr)
        return status;
    
    const UInt16 inputsCount = secondInputChannelIsActivated ? 4 : 2;
    Float32 *inputs[inputsCount];
    inputs[0] = firstInputBus->GetFloat32ChannelData(0);
    inputs[1] = firstInputBus->GetFloat32ChannelData(1);
    
    if (secondInputChannelIsActivated) {
        AUInputElement *secondInputBus = GetInput(1);
        status = secondInputBus->PullInput(ioActionFlags, timeStamp, 1, framesToProcess);
        if (status != noErr)
            return status;
        
        inputs[2] = secondInputBus->GetFloat32ChannelData(0);
        inputs[3] = secondInputBus->GetFloat32ChannelData(1);
    }
    else {
        inputs[2] = inputs[3] = nullptr;
    }
    
    const UInt16 outputsCount = 2;
    Float32 *outputs[outputsCount];
    outputs[0] = outputBus->GetFloat32ChannelData(0);
    outputs[1] = outputBus->GetFloat32ChannelData(1);
    
    if (JamTaba::jamTabaInstance) {
        updateHostState();
        JamTaba::jamTabaInstance->processAudio(inputs, outputs, inputsCount, outputsCount, framesToProcess);
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
                
            case kJamTabaGetHostState:
                outWritable = false;
                outDataSize = sizeof(AUHostState);
                return noErr;
            
            case kJamTabaSetInstance:
                 outWritable = true;
                 outDataSize = sizeof(JamTabaAUInterface *);
                return noErr;
                
            case kJamTabaGetInstance:
                outWritable = true;
                outDataSize = sizeof(JamTabaAUInterface *);
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
                
            case kJamTabaGetHostState:
            {
                updateHostState();
                *((AUHostState *)outData) = hostState;
                return noErr;
            }
                
            case kJamTabaGetInstance:
            {
                *(JamTabaAUInterface**) outData = JamTaba::jamTabaInstance;
                return noErr;
            }

  		}
	}
  	
	// if we've gotten this far, handles the standard properties
	return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}

bool JamTaba::StreamFormatWritable(AudioUnitScope scope, AudioUnitElement element)
{
	return IsInitialized() ? false : true;
}