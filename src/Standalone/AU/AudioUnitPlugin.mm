#include "AudioUnitPlugin.h"

#include <Cocoa/Cocoa.h>
#include "log/Logging.h"
#include "audio/core/SamplesBuffer.h"

#include "PublicUtility/CAStreamBasicDescription.h"
#include "PublicUtility/CABufferList.h"

#include <QtGlobal>
#include <QLayout>
#include <QDialog>

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
            id factoryInstance = [[[factoryClass alloc] init] autorelease];
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
      internalOutBuffer(2, blockSize),
      viewContainer(nullptr),
      wantsMidiMessages(AudioUnitPlugin::audioUnitWantsMidi(au))
{

    qDebug() << name << " wants midi: " << wantsMidiMessages;

    UInt32 propertySize = 0;
    Boolean writable;
    UInt32 numChannelInfos = 0;

    OSStatus status = AudioUnitGetPropertyInfo (audioUnit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, 0, &propertySize, &writable);
    if (status == noErr && propertySize > 0)
    {
        numChannelInfos = propertySize / sizeof (AUChannelInfo);
        AUChannelInfo infos[numChannelInfos];
        propertySize = static_cast<UInt32> (sizeof (AUChannelInfo) * static_cast<size_t> (numChannelInfos));

        OSStatus status = AudioUnitGetProperty (audioUnit, kAudioUnitProperty_SupportedNumChannels, kAudioUnitScope_Global, 0, &infos, &propertySize);
        if (status == noErr) {
                qDebug() << "supported ins:" << infos[0].inChannels << " outs:" << infos[0].outChannels;
        }
        else {
            qDebug() << "ERRO: " << status;
        }
    }
    else {
        qDebug() << "erro obtendo property info" << status;
    }

    initializeMaximumFramesPerSlice(blockSize);

    inputs = initializeChannels(kAudioUnitScope_Input);
    outputs = initializeChannels(kAudioUnitScope_Output);
    qDebug() << getName() << " channels initialized - inputs:" << inputs << ", outputs:" << outputs;

    initializeCallbacks();

    if (outputs == 1)
        internalOutBuffer.setToMono(); // default output is stereo

    Float64 sampleRate = static_cast<Float64>(initialSampleRate);
    initializeSampleRate(sampleRate);

    if (hasInputs())
        initializeStreamFormat(kAudioUnitScope_Input, inputs, sampleRate);

    if (hasOutputs())
        initializeStreamFormat(kAudioUnitScope_Output, outputs, sampleRate);

    initializeChannelLayout(kAudioUnitScope_Input);
    initializeChannelLayout(kAudioUnitScope_Output);

}

void AudioUnitPlugin::initializeChannelLayout(AudioUnitScope scope)
{
    AudioChannelLayout layout;
    const UInt32 minDataSize = sizeof (layout) - sizeof (AudioChannelDescription);
    UInt32 dataSize = minDataSize;

    OSStatus err = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_AudioChannelLayout, scope, 0, &layout, &dataSize);
    bool supportsLayouts = (err == noErr && dataSize >= minDataSize);
    if (supportsLayouts) {
        qDebug() << "setting layout in scope " << scope;
        AudioChannelLayout layout;
        layout.mChannelBitmap = 0;
        layout.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;
        //layout.mNumberChannelDescriptions = 1;
        //layout.mChannelDescriptions[0].mChannelFlags = kAudioUnit

        err = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_AudioChannelLayout, scope, 0, &layout, sizeof(AudioChannelLayout));
        if (err) {
            qCritical() << "Error setting channels layout! " << err;
        }
    }
    else {
        qDebug() << "Layout is not supported in scope " << scope << " OSStatus:" << err;
    }

}

bool AudioUnitPlugin::audioUnitWantsMidi(AudioUnit audioUnit)
{
    AudioComponentDescription desc;
    AudioComponentGetDescription(AudioComponentInstanceGetComponent(audioUnit), &desc);
    return desc.componentType == kAudioUnitType_MusicDevice
                            || desc.componentType == kAudioUnitType_MusicEffect;
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

    UInt32 bus = 0;
    OSStatus status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, scope, bus, &stream, sizeof(stream));
    if (status) {
       qCritical() << "Error setting stream format in audio unit OSStatus:" << status << " scope:" << scope << " ";
       NSLog (@"bus %d scope %d. rate: %f, %ld channels, %ld bits per channel", bus, scope, stream.mSampleRate, stream.mChannelsPerFrame, stream.mBitsPerChannel);

    }
    else {
        qDebug() << "Stream audio format setted in " << getName() << ", scope " << scope << " sr:" << stream.mSampleRate << " channels per frame:" << stream.mChannelsPerFrame;
    }
}

void AudioUnitPlugin::initializeSampleRate(Float64 initialSampleRate)
{

    Float64 sr = initialSampleRate;
    if (hasInputs()) {
        OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &sr, sizeof(Float64));
        if (status != noErr)
            qCritical() << "Error setting sample rate in input Status:" << status;
        else
            qDebug() << "Sample rate setted to " << initialSampleRate << " in " << getName() << " input scope";
    }

    if (hasOutputs()) {
        OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, 0, &sr, sizeof(Float64));
        if (status != noErr)
            qCritical() << "Error setting sample rate in Ouput Status:" << status;
        else
            qDebug() << "Sample rate setted to " << initialSampleRate << " in " << getName() << " output scope";
    }
}

void AudioUnitPlugin::initializeMaximumFramesPerSlice(UInt32 maxFrames)
{
    OSStatus status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_MaximumFramesPerSlice, kAudioUnitScope_Global, 0,
                         &maxFrames, sizeof(UInt32));
    if (status != noErr)
        qCritical() << "Error setting maximumFramesPerSlice status:" << status;
    else
        qDebug() << getName() << " - maximumFramesPerSlice setted to " << maxFrames;
}

void AudioUnitPlugin::initializeCallbacks()
{
    OSStatus status = noErr;

    if (hasInputs()) { // some plugins like Apple AU Sampler are ZERO inputs
        // set AU input callback
        AURenderCallbackStruct renderCallbackInfo;

        renderCallbackInfo.inputProcRefCon = this;
        renderCallbackInfo.inputProc = getInputCallback;

        status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
                          0, //assuming we have just one input bus
                          &renderCallbackInfo,
                          sizeof(renderCallbackInfo));

        if (status != noErr)
            qCritical() << "Error setting render callback in Audio Unit " << name << " => " << path << " OSStatus:" << status;
        else
            qDebug() << "Render callback setted in " << getName();
    }

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
    else
        qDebug() << "Host callback setted in " << getName();
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
    Boolean canChangeBus = false;

    OSStatus status = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_BusCount, kAudioUnitScope_Input, 0, &channels, &size);
    if (status == noErr) {
        status = AudioUnitGetPropertyInfo(audioUnit, kAudioUnitProperty_BusCount, scope, 0, &size, &canChangeBus);
        if (status != noErr)
            qCritical() << "Error getting propertyInfo to kAudioUnitProperty_BusCount OSStatus:" << status;

        if (canChangeBus && channels != 2) {
            channels = 2;
            status = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_BusCount, scope, 0, &channels, sizeof(channels));
            if (status != noErr)
                qCritical() << "Error setting kAudioUnitProperty_BusCount in scope " << scope << " OSStatus:" << status;
            else
                qDebug() << "Setting kAudioUnitProperty_BusCount in scope " << scope << " to " << channels;
        }
        else {
            qDebug() << "No changing kAudioUnitProperty_BusCount in scope " << scope << ", canChangeElements:" << canChangeBus << ", channels:" << channels;
        }
    }
    else {
        qCritical() << "Error getting kAudioUnitProperty_BusCount. OSStatus:" << status;
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

    bufferList = (AudioBufferList*)malloc(sizeof(*bufferList) + ((outputs-1) * sizeof(AudioBuffer)));
    bufferList->mNumberBuffers = outputs;

    OSStatus status = AudioUnitInitialize(audioUnit);

    if (status != noErr)
        qCritical() << "Error initializing audio unit OSStatus: " << status;
}

class ViewContainer : public QMacCocoaViewContainer
{
public:

    ViewContainer(NSView *cocoaView)
        : QMacCocoaViewContainer(cocoaView)
    {
        setAttribute(Qt::WA_DeleteOnClose);

        //add a listener for NSView resize
        CFNotificationCenterAddObserver
            (
                CFNotificationCenterGetLocalCenter(),
                this,
                &resizeHandler,
                CFSTR("NSViewFrameDidChangeNotification"),
                cocoaView,
                CFNotificationSuspensionBehaviorDeliverImmediately
            );
    }

    ~ViewContainer()
    {
        CFNotificationCenterRemoveEveryObserver
            (
                CFNotificationCenterGetLocalCenter(),
                this
            );
    }

private:

    static void resizeHandler(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef userInfo)
    {
        Q_UNUSED(userInfo)
        Q_UNUSED(center)
        Q_UNUSED(name)
        Q_UNUSED(object)

        ViewContainer *viewContainer = static_cast<ViewContainer *>(observer);
        if (viewContainer) {
           NSView *view = viewContainer->cocoaView();
           if (view) {
                auto mask = [view autoresizingMask];
                //QPoint viewContainerPosition(viewContainer->pos());
                [view setAutoresizingMask: NSViewNotSizable];
                NSRect frame = [view frame];
                viewContainer->resize(frame.size.width, frame.size.height);
                [view setAutoresizingMask:mask];
                //viewContainer->move(viewContainerPosition);
           }
        }
    }
};

void AudioUnitPlugin::openEditor(const QPoint &centerOfScreen)
{
    if (!viewContainer) { // editor window is not created yet
        NSView *cocoaView = createAudioUnitView(audioUnit);

       if (cocoaView) {
            viewContainer = new ViewContainer(cocoaView);
            viewContainer->setWindowTitle(getName());

            QObject::connect(viewContainer, &QDialog::destroyed, [=](){
                viewContainer = nullptr; // reset the view container when editor is closed and destroyed
            });

        }
        else {
           qCritical() << "Cocoa view is null!";
           viewContainer = nullptr; // just in case :)
           return;
        }
    }

    if (!viewContainer) // plugin doesn´t have a cocoa view
        return;

    if (!viewContainer->isVisible()) {
        int viewX = qMax(0, centerOfScreen.x() - viewContainer->width()/2);
        int viewY = qMax(0, centerOfScreen.y() - viewContainer->height()/2);

        viewContainer->move(QPoint(viewX, viewY));
    }

    viewContainer->raise();
    viewContainer->activateWindow();
    viewContainer->show();

}

void AudioUnitPlugin::closeEditor()
{
    if (viewContainer) {
        viewContainer->close();
    }
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

    if (wantsMidiMessages && !midiBuffer.isEmpty())
    {
        UInt32 midiEventPosition = 0; // in jamtaba all MIDI messages are real time
        for (const Midi::MidiMessage &message : midiBuffer) {
            MusicDeviceMIDIEvent(audioUnit, message.getStatus(), message.getData1(),
                                                            message.getData2(), midiEventPosition);

        }
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
;
