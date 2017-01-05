#include "AudioUnitPlugin.h"

#include <Cocoa/Cocoa.h>
#include "log/Logging.h"
#include "audio/core/SamplesBuffer.h"

#include "PublicUtility/CAStreamBasicDescription.h"
#include "PublicUtility/CABufferList.h"

#include <QtGlobal>
#include <QMacCocoaViewContainer>

#include <cmath>

using namespace AU;


NSView *createAudioUnitView(AudioUnit audioUnit)
{

    AudioUnitCocoaViewInfo cocoaViewInfo;
    UInt32 dataSize = sizeof(cocoaViewInfo);

    //request the cocoa view
    OSStatus status  = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_CocoaUI,
                                   kAudioUnitScope_Global, 0, &cocoaViewInfo, &dataSize);

    // a cocoa view is available?
    if (status == noErr) {

        CFURLRef cocoaViewBundlePath = cocoaViewInfo.mCocoaAUViewBundleLocation;
        CFStringRef factoryClassName = cocoaViewInfo.mCocoaAUViewClass[0];
        NSBundle * viewBundle = [NSBundle bundleWithURL:(NSURL *)cocoaViewBundlePath];

        if(viewBundle) {
            Class factoryClass = [viewBundle classNamed:(NSString *)factoryClassName];
            auto factoryInstance = [[[factoryClass alloc] init] autorelease];
            NSView *view = [factoryInstance uiViewForAudioUnit:audioUnit withSize:NSZeroSize];
            return view;
        }
        else {
            qCritical() << "Can´t obtain CocoaView bundle!";
        }
    }
    else {
        qCritical() << "No CocoaView available! OSStatus:" << status;
    }

    return nullptr;
}


AudioUnitPlugin::AudioUnitPlugin(const QString &name, const QString &path, AudioUnit au, int initialSampleRate, int blockSize)
    : Audio::Plugin(name),
      audioUnit(au),
      path(path),
      bufferList(nullptr),
      currentInputBuffer(nullptr),
      internalOutBuffer(2, blockSize)
{

    initializeCallbacks();

    initializeMaximumFramesPerSlice(blockSize);

    inputs = initializeChannels(kAudioUnitScope_Input);
    outputs = initializeChannels(kAudioUnitScope_Output);

    if (outputs == 1)
        internalOutBuffer.setToMono(); // default output is stereo

    Float64 sampleRate = static_cast<Float64>(initialSampleRate);
    initializeSampleRate(sampleRate);

    initializeStreamFormat(kAudioUnitScope_Input, inputs, sampleRate);
    initializeStreamFormat(kAudioUnitScope_Output, outputs, sampleRate);

}

void AudioUnitPlugin::initializeStreamFormat(AudioUnitScope scope, UInt32 channels, Float64 sampleRate)
{
    AudioStreamBasicDescription stream;
    stream.mSampleRate      = sampleRate;
    stream.mFormatID         = kAudioFormatLinearPCM;
    stream.mFormatFlags      = kAudioFormatFlagsNativeFloatPacked | kAudioFormatFlagIsNonInterleaved | kAudioFormatFlagsNativeEndian;
    stream.mFramesPerPacket  = 1;
    stream.mBytesPerPacket   = 4;
    stream.mBytesPerFrame    = 4;
    stream.mBitsPerChannel   = 32;
    stream.mChannelsPerFrame = static_cast<UInt32> (channels);
    stream.mReserved = 0;

    OSStatus status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, scope, 0, &stream, sizeof(stream));
    if (status) {
       qCritical() << "Error setting stream format in audio unit OSStatus:" << status;
    }
}

void AudioUnitPlugin::initializeSampleRate(Float64 initialSampleRate)
{
    Float64 sr = initialSampleRate;
    OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &sr, sizeof(Float64));
    if (status != noErr)
        qCritical() << "Error setting sample rate in input Status:" << status;

    status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, 0, &sr, sizeof(Float64));
    if (status != noErr)
        qCritical() << "Error setting sample rate in Ouput Status:" << status;
}

void AudioUnitPlugin::initializeMaximumFramesPerSlice(UInt32 maxFrames)
{
    OSStatus status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0,
                         &maxFrames, sizeof(UInt32));
    if (status != noErr)
        qCritical() << "Error setting maximumFramesPerSlice status:" << status;

}

void AudioUnitPlugin::initializeCallbacks()
{
    // set AU input callback
    AURenderCallbackStruct renderCallbackInfo;

    renderCallbackInfo.inputProcRefCon = this;
    renderCallbackInfo.inputProc = getInputCallback;

    OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
                          0, //assuming we have just one input bus
                          &renderCallbackInfo,
                          sizeof(renderCallbackInfo));

    if (status != noErr)
        qCritical() << "Error setting render callback in Audio Unit " << name << " => " << path;



    // set host callback
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
}

/**
 * @brief AudioUnitPlugin::adjustChannels Try set the scope channels to 2 (stereo) if possible. Return the real channels number of scope.
 * @param scope input or output scope
 * @return The number of channels in the scope
 */
UInt32 AudioUnitPlugin::initializeChannels(AudioUnitScope scope)
{
    UInt32 channels;
    UInt32 size = sizeof(channels);
    Boolean writable = false;

    OSStatus status = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Input, 0, &channels, &size);
    if (status == noErr) {
        if (channels != 2) {

            status = AudioUnitGetPropertyInfo(audioUnit, kAudioUnitProperty_ElementCount, scope, 0, &size, &writable);
            if (status != noErr)
                qCritical() << "Error getting propertyInfo to ElementCount OSStatus:" << status;

            if (writable) {
                channels = 2;
                status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_ElementCount, kAudioUnitScope_Input, 0, &channels, sizeof(channels));
                if (status != noErr)
                    qCritical() << "Error setting ElementCount in input OSStatus:" << status;
            }
        }
    }
    else {
        qCritical() << "Error getting ElementCount. OSStatus:" << status;
    }
    return channels;
}

AudioUnitPlugin::~AudioUnitPlugin()
{
    AudioUnitUninitialize(audioUnit);
    AudioComponentInstanceDispose(audioUnit);

    delete bufferList;
}

void AudioUnitPlugin::start()
{
    if (bufferList)
        delete bufferList;

    static const quint32 channelCount = 1;

    bufferList = (AudioBufferList*)malloc(sizeof(*bufferList) + ((channelCount-1) * sizeof(AudioBuffer)));
    bufferList->mNumberBuffers = channelCount;

    OSStatus status = AudioUnitInitialize(audioUnit);

    if (status != noErr)
        qCritical() << "Error initializing audio unit OSStatus: " << status;
}

void AudioUnitPlugin::openEditor(const QPoint &centerOfScreen)
{
    NSView *cocoaView = createAudioUnitView(audioUnit);
    if (cocoaView) {
        QMacCocoaViewContainer *cocoaViewContainer = new QMacCocoaViewContainer(cocoaView);
        cocoaViewContainer->show();
    }
    else {
        qCritical() << "Cocoa view is null!";
    }
}

void AudioUnitPlugin::closeEditor()
{

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


OSStatus AudioUnitPlugin::getInputCallback (void* hostRef, AudioUnitRenderActionFlags* ioActionFlags,
                                            const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
                                            UInt32 inNumberFrames, AudioBufferList* ioData)
{
    Q_UNUSED(ioActionFlags)
    Q_UNUSED(inBusNumber)
    Q_UNUSED(inTimeStamp)

    AudioUnitPlugin *instance = static_cast<AudioUnitPlugin *>(hostRef);
    instance->copyBufferContent(instance->currentInputBuffer, ioData, inNumberFrames);

    return noErr;
}

void AudioUnitPlugin::process(const Audio::SamplesBuffer &inBuffer, Audio::SamplesBuffer &outBuffer,
                     const QList<Midi::MidiMessage> &midiBuffer)
{

    AudioUnitRenderActionFlags flags = 0;
    static AudioTimeStamp timeStamp;

    UInt32 frames = outBuffer.getFrameLenght();

    if (!bufferList)
        qFatal("Buffer list is null");

    currentInputBuffer = &inBuffer;

    // prepare the output AudioBufferList
    internalOutBuffer.setFrameLenght(frames);
    for (quint8 i = 0; i < bufferList->mNumberBuffers; i++) {
        bufferList->mBuffers[i].mNumberChannels = 1;
        bufferList->mBuffers[i].mDataByteSize = (UInt32) (sizeof (float) * (size_t) frames);
        bufferList->mBuffers[i].mData = internalOutBuffer.getSamplesArray(i);
    }

    OSStatus status = AudioUnitRender(audioUnit, &flags, &timeStamp,
                     0, //assuming just one output bus
                     frames, bufferList);

    if (status != noErr) {
        qWarning() << "Error rendering audio unit OSStatus: " << status;
        internalOutBuffer.zero();
    }

    outBuffer.set(internalOutBuffer);

    timeStamp.mHostTime++;
    timeStamp.mSampleTime += frames;

}

void AudioUnitPlugin::copyBufferContent(const Audio::SamplesBuffer *input, AudioBufferList *abl, quint32 frames)
{
    const quint8 channels = qMin((int)abl->mNumberBuffers, input->getChannels());
    const size_t bytesToCopy = sizeof(float) * frames;
    for(quint8 c = 0; c < channels; ++c) {
        std::memcpy(abl->mBuffers[c].mData, input->getSamplesArray(c), bytesToCopy);
    }
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
