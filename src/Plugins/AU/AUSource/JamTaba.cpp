#include "JamTaba.h"
#include "MainControllerPlugin.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainControllerAU : public MainControllerPlugin
{
public:
    MainControllerAU(const Persistence::Settings &settings, JamTabaPlugin *plugin)
        : MainControllerPlugin(settings, plugin)
    {
        
    }
    
    QString getJamtabaFlavor() const override
    {
        return "AU Plugin";
    }
    
    void resizePluginEditor(int newWidth, int newHeight) override
    {
        
    }
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Standard DSP AudioUnit implementation

AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, JamTaba)


JamTaba *JamTaba::instance = nullptr;

JamTaba::JamTaba(AudioUnit component)
	: AUEffectBase(component),
      JamTabaPlugin(2, 2)
{
	if (instance == nullptr) {
        instance = this;
        
    }
    qDebug() << "AUDIO UNIT: " << component;
}

MainControllerPlugin * JamTaba::createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const
{
    return new MainControllerAU(settings, plugin);
}

qint32 JamTaba::getStartPositionForHostSync() const
{
    return 0; // TODO implementar
}

bool JamTaba::hostIsPlaying() const
{
    return true; //TODO implementar
}

int JamTaba::getHostBpm() const
{
    return 120; //TODO implementar
}

float JamTaba::getSampleRate() const
{
    return 44100; // TODO implementar
}

QString JamTaba::getHostName()
{
    return "Implementar!"; //http://lists.apple.com/archives/coreaudio-api/2007/Mar/msg00167.html
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
	
    // start the configurator
    //Configurator *configurator = Configurator::getInstance();
    //if (!configurator->setUp())
        //qCWarning(jtConfigurator) << "JTBConfig->setUp() FAILED !";
    
    JamTabaPlugin::initialize();
 	
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