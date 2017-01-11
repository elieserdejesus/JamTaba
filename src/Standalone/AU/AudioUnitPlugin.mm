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
                [view setAutoresizingMask: NSViewNotSizable];
                NSRect frame = [view frame];
                viewContainer->resize(frame.size.width, frame.size.height);
                frame.origin.x = frame.origin.y = 0;
                [view setAutoresizingMask:mask];
                [view setFrame:frame];

           }
        }
    }
};


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


quint8 AudioUnitPlugin::getBusCount(AudioUnit comp, AudioUnitScope scope)
{
    UInt32 count;
    UInt32 countSize = sizeof(count);

    OSStatus err = AudioUnitGetProperty (comp, kAudioUnitProperty_ElementCount, scope, 0, &count, &countSize);

    if (err) {
        qCritical() << "Error getting element count scope:" << scope << " error:" << err;
        return static_cast<quint8>(0);
    }

    return static_cast<quint8> (count);
}

Audio::PluginDescriptor AU::createPluginDescriptor(const QString &name, const QString &path)
{
    // name is formatted as MANUFACTURER: AU plugin name
    QString manufacturer = "";
    QString pluginName = name;
    int index = name.indexOf(":");
    if (index > 0) {
        manufacturer = name.left(index);
        pluginName = name.right(name.size() - (index + 1)).trimmed();
    }
    auto category = Audio::PluginDescriptor::AU_Plugin;
    return Audio::PluginDescriptor(pluginName, category, manufacturer, path);
}

AudioUnitPlugin::AudioUnitPlugin(const QString &name, const QString &path, AudioUnit au, int initialSampleRate, int blockSize)
    : Audio::Plugin(AU::createPluginDescriptor(name, path)),
      audioUnit(au),
      path(path),
      bufferList(nullptr),
      currentInputBuffer(nullptr),
      internalOutBuffer(2, 4096),
      viewContainer(nullptr),
      wantsMidiMessages(AudioUnitPlugin::audioUnitWantsMidi(au)),
      hasInputs(AudioUnitPlugin::getBusCount(au, kAudioUnitScope_Input) > 0),
      hasOutputs(AudioUnitPlugin::getBusCount(au, kAudioUnitScope_Output) > 0)
{

    initializeMaximumFramesPerSlice(blockSize);

    initializeCallbacks();

    Float64 sampleRate = static_cast<Float64>(initialSampleRate);
    initializeSampleRate(sampleRate);

    if (hasInputs)
        initializeStreamFormat(kAudioUnitScope_Input, 2, sampleRate);

    if (hasOutputs)
        initializeStreamFormat(kAudioUnitScope_Output, 2, sampleRate);
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
    stream.mFormatFlags      = kAudioFormatFlagsAudioUnitCanonical;
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
    }
}

void AudioUnitPlugin::initializeSampleRate(Float64 initialSampleRate)
{

    Float64 sr = initialSampleRate;
    if (hasInputs) {
        OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Input, 0, &sr, sizeof(Float64));
        if (status != noErr)
            qCritical() << "Error setting sample rate in input Status:" << status;
    }

    if (hasOutputs) {
        OSStatus status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SampleRate, kAudioUnitScope_Output, 0, &sr, sizeof(Float64));
        if (status != noErr)
            qCritical() << "Error setting sample rate in Ouput Status:" << status;
    }
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
    OSStatus status = noErr;

    if (hasInputs) { // some plugins like Apple AU Sampler are ZERO inputs

        AURenderCallbackStruct renderCallbackInfo;

        renderCallbackInfo.inputProcRefCon = this;
        renderCallbackInfo.inputProc = inputCallback;

        status = AudioUnitSetProperty (audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input,
                          0, //assuming we have just one input bus
                          &renderCallbackInfo,
                          sizeof(renderCallbackInfo));

        if (status != noErr)
            qCritical() << "Error setting render callback in Audio Unit " << name << " => " << path << " OSStatus:" << status;
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

    const quint8 channels = 2; // in jamtaba the stream is always stereo (left + right)
    bufferList = (AudioBufferList*)malloc(sizeof(*bufferList) + ((channels-1) * sizeof(AudioBuffer)));
    bufferList->mNumberBuffers = channels;

    OSStatus status = AudioUnitInitialize(audioUnit);

    if (status != noErr)
        qCritical() << "Error initializing audio unit OSStatus: " << status;
}

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


OSStatus AudioUnitPlugin::inputCallback (void* hostRef, AudioUnitRenderActionFlags* ioActionFlags,
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

    if (!bufferList) {
        qCritical() << "Buffer list is null";
        return;
    }

    currentInputBuffer = &inBuffer;

    // prepare the output AudioBufferList
    quint8 channels = qMin(static_cast<int>(bufferList->mNumberBuffers), internalOutBuffer.getChannels());
    for (quint8 i = 0; i < channels; i++) {
        bufferList->mBuffers[i].mNumberChannels = 1; // each buffer contain one audio channel (left or right, for example)
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

    UInt32 bus = 0; //using just one out bus
    OSStatus status = AudioUnitRender(audioUnit, &flags, &timeStamp, bus, frames, bufferList);

    if (status != noErr) {
        qWarning() << "Error rendering audio unit " << getName() << " OSStatus: " << status;
        return;
    }


    /**
        AUs are processing input samples and replacing the output buffer with these processed samples.
    AUis are generating output samples directly, without touch the input buffer, and this make sense.
    In insert chain the input buffer is the output generated by the previous plugin. So, if AUis are
    ignoring this input (the samples generated by the previous plugin) they will generate a fresh output
    and replacing the last generated samples. The result is just the last AUi in the chain can be heard.
    */

    if(isVirtualInstrument()){
        outBuffer.add(internalOutBuffer);// AUi add and preserve the last generated output samples
    }
    else{
        outBuffer.set(internalOutBuffer);// AUs are replacing
    }


    timeStamp.mHostTime++;
    timeStamp.mSampleTime += frames;

}

bool AudioUnitPlugin::isVirtualInstrument() const
{
    return wantsMidiMessages;
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
        qCritical() << "Audio Unit plugin path string need exactly 3 parts! (" << path << ")";
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


QList<Audio::PluginDescriptor> AU::scanAudioUnitPlugins()
{
    static QList<OSType> supportedAudioUnitTypes;
    supportedAudioUnitTypes << kAudioUnitType_MusicDevice;
    supportedAudioUnitTypes << kAudioUnitType_MusicEffect;
    supportedAudioUnitTypes << kAudioUnitType_Effect;
    supportedAudioUnitTypes << kAudioUnitType_Mixer;
    supportedAudioUnitTypes << kAudioUnitType_Generator;
    supportedAudioUnitTypes << kAudioUnitType_MIDIProcessor;

    AudioComponent comp = nullptr;
    QList<Audio::PluginDescriptor> descriptors;
    do {
        AudioComponentDescription desc;
        desc.componentType = OSType(0);
        desc.componentSubType = OSType(0);
        desc.componentManufacturer = OSType(0);
        desc.componentFlags = 0;
        desc.componentFlagsMask = 0;

        comp = AudioComponentFindNext(comp, &desc);
        if (comp) {
            AudioComponentInstance instance;
            OSStatus status = AudioComponentInstanceNew(comp, &instance);
            if (status == noErr) {
                status = AudioComponentGetDescription(comp, &desc);
                if (status == noErr) {

                    if (!supportedAudioUnitTypes.contains(desc.componentType))
                        continue; // skip unsupported types

                    QString type(AU::osTypeToString(desc.componentType));
                    QString subType(AU::osTypeToString(desc.componentSubType));
                    QString manufacturer(AU::osTypeToString(desc.componentManufacturer));

                    CFStringRef cfName;
                    status = AudioComponentCopyName(comp, &cfName);
                    if (status == noErr) {
                        QString name = QString::fromCFString(cfName);
                        QString path(type + ":" + subType + ":" + manufacturer);
                        descriptors.append(createPluginDescriptor(name, path));
                    }
                }
                AudioComponentInstanceDispose(instance);
            }

        }
    }
    while(comp);

    return descriptors;
}
