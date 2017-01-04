#include "AudioUnitPlugin.h"

#include <Cocoa/Cocoa.h>
#include "log/Logging.h"
#include "audio/core/SamplesBuffer.h"

#include "PublicUtility/CAStreamBasicDescription.h"

#include <QThread>

using namespace AU;

AudioUnitPlugin::AudioUnitPlugin(const QString &name, const QString &path, AudioUnit au, int initialSampleRate, int blockSize)
    : Audio::Plugin(name),
      audioUnit(au),
      path(path),
      bufferList(nullptr)
{

//    AURenderCallbackStruct renderCallbackInfo;

//    renderCallbackInfo.inputProcRefCon = this;
//    renderCallbackInfo.inputProc = getInputCallback;

//    OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
//                          0, //assumint we have just one input bus
//                          &renderCallbackInfo,
//                          sizeof(renderCallbackInfo));
    OSStatus status = noErr;
    if (status != noErr)
        qCritical() << "Error setting render callback in Audio Unit " << name << " => " << path;

    HostCallbackInfo hostCallbackInfo;

    hostCallbackInfo.hostUserData = this;
    hostCallbackInfo.beatAndTempoProc = getBeatAndTempoCallback;
    hostCallbackInfo.musicalTimeLocationProc = getMusicalTimeLocationCallback;
    hostCallbackInfo.transportStateProc = getTransportStateCallback;
    hostCallbackInfo.transportStateProc2 = 0;

    status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_HostCallbacks,
                          kAudioUnitScope_Global, 0, &hostCallbackInfo, sizeof (hostCallbackInfo));

    if (status != noErr)
        qCritical() << "Error setting host callback in Audio Unit " << name << " => " << path;

    setSampleRate(initialSampleRate);

    UInt32 maxFrames = blockSize;
    status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0,
                         &maxFrames, sizeof(UInt32));

    if (status != noErr)
        qCritical() << "Error setting maximumFramesPerSlice " << name << " => " << path;


    Float64 sr = initialSampleRate;
    status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &sr, sizeof(Float64));
    if (status != noErr)
        qCritical() << "Error setting sample rate in input " << name << " => " << path;

    status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, 0, &sr, sizeof(Float64));
    if (status != noErr)
        qCritical() << "Error setting sample rate in Ouput " << name << " => " << path;

    CAStreamBasicDescription streamFormat(initialSampleRate, 2, CAStreamBasicDescription::kPCMFormatFloat32, false);

     status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamFormat, sizeof(streamFormat));
     if (status) {
         qCritical() << "Error setting stream format in audio unit INPUT OSStatus:" << status;
     }

     status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 0, &streamFormat, sizeof(streamFormat));
     if (status) {
        qCritical() << "Error setting stream format in audio unit OUTPUT OSStatus:" << status;
     }

}

AudioUnitPlugin::~AudioUnitPlugin()
{
    AudioUnitUninitialize(audioUnit);
    AudioComponentInstanceDispose(audioUnit);
}

void AudioUnitPlugin::start()
{
    if (bufferList)
        free(bufferList);

    static const quint32 channelCount = 2;

    qDebug() << "Creating buffer list!";
    bufferList = (AudioBufferList*)malloc(sizeof(*bufferList) + ((channelCount-1) * sizeof(AudioBuffer)));
    bufferList->mNumberBuffers = channelCount;

    OSStatus status = AudioUnitInitialize(audioUnit);

    if (status != noErr)
        qCritical() << "Error initializing audio unit OSStatus: " << status;
}

void AudioUnitPlugin::openEditor(const QPoint &centerOfScreen)
{

}

void AudioUnitPlugin::closeEditor()
{

}

OSStatus AudioUnitPlugin::getInputCallback (void* hostRef, AudioUnitRenderActionFlags* ioActionFlags,
                                            const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
                                            UInt32 inNumberFrames, AudioBufferList* ioData)
{
    qDebug() << "Render callback in thread " << QThread::currentThreadId();
    qDebug() << "render input call back in:BusNumber" << inBusNumber << " frames:" << inNumberFrames << " bufferListChannels: " << ioData->mNumberBuffers;

    return noErr;
}

OSStatus AudioUnitPlugin::getBeatAndTempoCallback (void* hostRef, Float64* outCurrentBeat, Float64* outCurrentTempo)
{
    qDebug() << "Beat and Tempo callback";

    static Float64 beat = 0;
    static Float64 tempo = 110;
    outCurrentBeat = &beat;
    outCurrentTempo = &tempo;

    return noErr;
}

OSStatus AudioUnitPlugin::getMusicalTimeLocationCallback (void* hostRef, UInt32* outDeltaSampleOffsetToNextBeat,
                                                    Float32* outTimeSig_Numerator, UInt32* outTimeSig_Denominator,
                                                    Float64* outCurrentMeasureDownBeat)
{
    qDebug() << "Musical Time Location callback";

    static UInt32 deltaSampleOffsetToNextBeat = 0;
    static Float32 timeSigNumerator = 4;
    static UInt32 timeSigDenominator = 4;
    static Float64 currentMeasureDownBeat = 0;

    outDeltaSampleOffsetToNextBeat = &deltaSampleOffsetToNextBeat;
    outTimeSig_Numerator = &timeSigNumerator;
    outTimeSig_Denominator = &timeSigDenominator;
    outCurrentMeasureDownBeat = &currentMeasureDownBeat;

    return noErr;
}

OSStatus AudioUnitPlugin::getTransportStateCallback (void* hostRef, Boolean* outIsPlaying, Boolean* outTransportStateChanged,
                                               Float64* outCurrentSampleInTimeLine, Boolean* outIsCycling,
                                               Float64* outCycleStartBeat, Float64* outCycleEndBeat)
{
    qDebug() << "Transport state callback";

    static Boolean isPLaying = true;
    static Boolean transportStateChanged = false;
    static Float64 currentSampleInTimeLine = 0;
    static Boolean isCycling = false;
    static Float64 cycleStartBeat = 0;
    static Float64 cycleEndBeat = 0;

    outIsCycling = &isCycling;
    outIsPlaying = &isPLaying;
    outTransportStateChanged = &transportStateChanged;
    outCurrentSampleInTimeLine = &currentSampleInTimeLine;
    outCycleStartBeat = &cycleStartBeat;
    outCycleEndBeat = &cycleEndBeat;

    return noErr;
}

void AudioUnitPlugin::process(const Audio::SamplesBuffer &inBuffer, Audio::SamplesBuffer &outBuffer,
                     const QList<Midi::MidiMessage> &midiBuffer)
{

    qDebug() << "processing audio in thread " << QThread::currentThreadId();

    AudioUnitRenderActionFlags flags = 0;
    static AudioTimeStamp timeStamp;

    UInt32 frames = outBuffer.getFrameLenght();

    if (!bufferList)
        qFatal("Buffer list is null");

    OSStatus status = AudioUnitRender (audioUnit, &flags, &timeStamp,
                     0, //assuming just ont output bus
                     frames, bufferList);

    if (status != noErr) {
        qWarning() << "Error rendering audio unit OSStatus: " << status;
    }

    timeStamp.mHostTime++;

}

void AudioUnitPlugin::suspend()
{

}

void AudioUnitPlugin::resume()
{

}

void AudioUnitPlugin::updateGui()
{

}

void AudioUnitPlugin::setSampleRate(int newSampleRate)
{
    qDebug() << "Setting sample rate in Audio unit " << newSampleRate;

    //AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Global, 0, )

    //AudioUnitUninitialize(audioUnit);
    //qDebug() << "Audio Unit unitialized " << this->name;

    //UInt32 asbdSize = sizeof(AudioStreamBasicDescription);
    //AudioStreamBasicDescription asbd = {0};
    //AudioUnitGetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output,
    //                         0, &asbd, &asbdSize);

    //asbd.mSampleRate = newSampleRate;

    //qDebug() << "Setting sample in audio unit output";
    // set output
    //AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output,
    //                     0, &asbd, asbdSize);

    //qDebug() << "Setting sample in audio unit input";
    //set input
    //AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input,
    //                     0, &asbd, asbdSize);

    //qDebug() << "Re-initializing audio unit";
    //OSStatus status = AudioUnitInitialize(audioUnit);

    //if (status != noErr)
    //    qCritical() << "Error initializing audio unit OSStatus: " << status;

    //qDebug() << "Done!";
}

//QDialog * AudioUnitPlugin::createEditorWindow(AudioUnit unit)
//{

//   AudioUnitCocoaViewInfo cocoaViewInfo;
//    UInt32 dataSize = sizeof(cocoaViewInfo);

//    //request the cocoa view
//    OSStatus status  = AudioUnitGetProperty(unit, kAudioUnitProperty_CocoaUI,
//                                   kAudioUnitScope_Global, 0, &cocoaViewInfo, &dataSize);

//    // a cocoa view is available?
//    if (status == noErr) {

//        CFURLRef cocoaViewBundlePath = cocoaViewInfo.mCocoaAUViewBundleLocation;
//        CFStringRef factoryClassName = cocoaViewInfo.mCocoaAUViewClass[0];
//        NSBundle * viewBundle = [NSBundle bundleWithURL:(NSURL *)cocoaViewBundlePath];

//        if(viewBundle) {
//            Class factoryClass = [viewBundle classNamed:(NSString *)factoryClassName];
//            id<AUCocoaUIBase> factoryInstance = [[[factoryClass alloc] init] autorelease];
//            NSView *view = [factoryInstance uiViewForAudioUnit:unit withSize:NSZeroSize];
//            return view;
//        }
//    }

//    return nullptr;
//}

AudioUnitPlugin *AU::audioUnitPluginfromPath(const QString &path, int initialSampleRate, int blockSize)
{
    AudioComponentDescription auDescription;
    if (!getComponentDescriptionFromPath(path, auDescription))
        return nullptr; // some problem when spliting path

    AudioComponent component = AudioComponentFindNext(nullptr, &auDescription);
    if (component) {
        qDebug() << "Component founded! " << path;
        AudioUnit audioUnit;
        OSStatus status = AudioComponentInstanceNew(component, &audioUnit);
        if (status == noErr) {
            CFStringRef name;
            status = AudioComponentCopyName(component, &name);
            if (status == noErr)
                return new AudioUnitPlugin(QString::fromCFString(name), path, audioUnit, initialSampleRate, blockSize);
            else
                qCritical() << "Error getting audio unit name! OSStatus: " << status;
        }
        else
        {
            qCritical() << "Error instantiating AU plugin " << path;
        }
    }
    else
    {
        qCritical() << "Error! Can´t find AU plugin for path " << path;
    }

    return nullptr;

}

QString AU::osTypeToString (OSType type)
{
    const wchar_t s[4] = { (wchar_t) ((type >> 24) & 0xff),
                                  (wchar_t) ((type >> 16) & 0xff),
                                  (wchar_t) ((type >> 8) & 0xff),
                                  (wchar_t) (type & 0xff) };
    return QString::fromWCharArray(s, 4);
}

//this piece of code was stoled from JUCE :) https://searchcode.com/codesearch/view/12071132/
OSType AU::stringToOSType (const QString& string)
{
    if (string.size() < 4)
        return OSType(0);

    std::string s = string.toStdString();
    return (((OSType) (unsigned char) s[0]) << 24)
         | (((OSType) (unsigned char) s[1]) << 16)
         | (((OSType) (unsigned char) s[2]) << 8)
         | ((OSType) (unsigned char) s[3]);
}

bool AU::getComponentDescriptionFromPath(const QString &path, AudioComponentDescription &desc)
{
    QStringList parts = path.split(":");
    if (parts.size() != 3) {
        qCritical() << "Audio Unit plugin path string need excatly 3 parts! (" << path << ")";
        return false;
    }

    OSType type = AU::stringToOSType(parts.at(0));
    OSType subType = AU::stringToOSType(parts.at(1));
    OSType manufacturer = AU::stringToOSType(parts.at(2));

    desc.componentType = type;
    desc.componentSubType = subType;
    desc.componentManufacturer = manufacturer;
    desc.componentFlags = desc.componentFlagsMask = 0;

    return true;
}
