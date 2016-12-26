#include "JamTabaAUPlugin.h"
#include <QMacNativeWidget>


JamTabaAUPlugin* JamTabaAUPlugin::instance = nullptr;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Listener::Listener(JamTabaAUPlugin *auPlugin)
    : auPlugin(auPlugin)
{
    //
}
    
void Listener::process(Float32 **inputs, Float32 **outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess, const AUHostState &hostState)
{
   auPlugin->process(inputs, outputs, inputsCount, outputsCount, framesToProcess, hostState);
}
    
void Listener::cleanUp()
{
    JamTabaAUPlugin::releaseInstance();
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class MainControllerAU : public MainControllerPlugin
{
public:
    MainControllerAU(const Persistence::Settings &settings, JamTabaPlugin *plugin)
        : MainControllerPlugin(settings, plugin)
    {

    }

    QString getJamtabaFlavor() const override
    {
        return QStringLiteral("AU Plugin");
    }

    void resizePluginEditor(int newWidth, int newHeight) override
    {
        JamTabaAUPlugin *jamTabaPlugin = dynamic_cast<JamTabaAUPlugin*>(this->plugin);
        jamTabaPlugin->resizeWindow(newWidth, newHeight);
    }
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

JamTabaAUPlugin::JamTabaAUPlugin(AudioUnit audioUnit)
    :JamTabaPlugin(4, 2),
    listener(new Listener(this)),
    audioUnit(audioUnit),
    initializing(true)
{
    UInt32 size = sizeof(AUHostState);
    AudioUnitGetProperty(audioUnit, kJamTabaGetHostState, kAudioUnitScope_Global, 0, &hostState, &size);
}

JamTabaAUPlugin::~JamTabaAUPlugin()
{
    finalize();
}

JamTabaAUPlugin* JamTabaAUPlugin::getInstance(AudioUnit unit)
{
    if (!JamTabaAUPlugin::instance) {
        JamTabaAUPlugin::instance = new JamTabaAUPlugin(unit);
        JamTabaAUPlugin::instance->initialize();
    }
    
    return JamTabaAUPlugin::instance;
}

void JamTabaAUPlugin::releaseInstance()
{
    if (JamTabaAUPlugin::instance) {
        delete JamTabaAUPlugin::instance;
        JamTabaAUPlugin::instance = nullptr;
    }
}

void JamTabaAUPlugin::initialize()
{
    JamTabaPlugin::initialize();
 
    mainWindow = nullptr;
    
    nativeView = nullptr;// createNativeView();
    
    initializing = false;
}

void JamTabaAUPlugin::finalize()
{
    if (nativeView && mainWindow) {
        
        // avoid a crash when host is closed and the user name or channel QLineEdit is focused
        QWidget *focusWidget = QApplication::focusWidget();
        if (focusWidget)
            focusWidget->clearFocus();
        
        nativeView->deleteLater();
    
        nativeView = nullptr;
        mainWindow = nullptr;
    }
}

void JamTabaAUPlugin::resizeWindow(int newWidth, int newHeight)
{
    if (initializing)
        return; //avoid a crash when initializing and try to use nativeView
    
    if (mainWindow && nativeView) {
        
        QSize newSize(newWidth, newHeight);
        mainWindow->resize(newSize);
        
        if (!nativeView->isVisible())
            return;
        
        //cast voodoo learned from http://lists.qt-project.org/pipermail/interest/2014-January/010655.html
        NSView *nativeWidgetView = (__bridge NSView *)reinterpret_cast<void*>(nativeView->winId());
        
        NSRect frame;
        frame.size.width  = mainWindow->width();
        frame.size.height = mainWindow->height();
        [nativeWidgetView setFrame: frame];
        NSView *parentView = [nativeWidgetView superview];
        [parentView setFrame: frame];

    }
}

void JamTabaAUPlugin::process(Float32 **inputs, Float32 **outputs, UInt16 inputsCount, UInt16 outputsCount, UInt32 framesToProcess, const AUHostState &hostState)
{
    this->hostState = hostState;
    
    if (!controller)
        return;
    
    if (controller->isPlayingInNinjamRoom()) {
        
        // ++++++++++ sync ninjam with host  ++++++++++++
        if (transportStartDetectedInHost()) {// user pressing play/start in host?
            NinjamControllerPlugin *ninjamController = controller->getNinjamController();
            if (ninjamController->isWaitingForHostSync()) {
                ninjamController->startSynchronizedWithHost(getStartPositionForHostSync());
            }
        }
    }
    
    // ++++++++++ Audio processing +++++++++++++++
    inputBuffer.setFrameLenght(framesToProcess);
    quint32 bytesToCopy = sizeof(float) * framesToProcess;
    quint8 channelsToCopy = qMin((quint8)inputBuffer.getChannels(), (quint8)inputsCount);
    for (int c = 0; c < channelsToCopy; ++c) {
        if (inputs[c] != nullptr)
            memcpy(inputBuffer.getSamplesArray(c), inputs[c], bytesToCopy);
    }
    
    outputBuffer.setFrameLenght(framesToProcess);
    outputBuffer.zero();
    
    controller->process(inputBuffer, outputBuffer, getSampleRate());
    
    channelsToCopy = qMin((quint8)outputBuffer.getChannels(), (quint8)outputsCount);
    for (int c = 0; c < channelsToCopy; ++c) {
        if (outputs[c] != nullptr)
            memcpy(outputs[c], outputBuffer.getSamplesArray(c), bytesToCopy);
    }
    
    // ++++++++++++++++++++++++++++++
    hostWasPlayingInLastAudioCallBack = hostIsPlaying();
}

MainControllerPlugin * JamTabaAUPlugin::createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const
{
    return new MainControllerAU(settings, plugin);
}

qint32 JamTabaAUPlugin::getStartPositionForHostSync() const
{
    
     qint32 startPosition = 0;
    
     double samplesPerBeat = (60.0 * hostState.sampleRate)/hostState.tempo;
     if (hostState.ppqPos > 0)
     {
         //when host start button is pressed (and the cursor is aligned in project start) ppqPos is positive in Reaper, but negative in  Cubase
         
         double cursorPosInMeasure = hostState.ppqPos - hostState.barStartPos;
         if (cursorPosInMeasure > 0.00000001) {
             //the 'cursor' in the host is not aligned in the measure start. Whe need shift the interval start a little bit. Comparing with 0.00000001 because when the 'cursor' is in 5th beat Reaper is returning barStartPos = 4.9999999 and ppqPos = 5
             
             double samplesUntilNextMeasure = (hostState.timeSigNumerator - cursorPosInMeasure) * samplesPerBeat;
             startPosition = -samplesUntilNextMeasure; //shift the start position to compensate the 'cursor' position
         }
     }
     else
     {  //host is returning negative values for timeInfo structure when start button is pressed
         startPosition = hostState.ppqPos * samplesPerBeat; //wil generate a negative value
     }
     return startPosition;
 
}

bool JamTabaAUPlugin::hostIsPlaying() const
{
    return hostState.playing;
}

int JamTabaAUPlugin::getHostBpm() const
{
    return hostState.tempo;
}

float JamTabaAUPlugin::getSampleRate() const
{
    return hostState.sampleRate;
}

QString JamTabaAUPlugin::CFStringToQString(CFStringRef str)
{

        if (!str)
            return QString();
        
        CFIndex length = CFStringGetLength(str);
        if (length == 0)
            return QString();
        
        QString string(length, Qt::Uninitialized);
        CFStringGetCharacters(str, CFRangeMake(0, length), reinterpret_cast<UniChar *>
                              (const_cast<QChar *>(string.unicode())));
        return string;
    
}

QString JamTabaAUPlugin::getHostName()
{
    AUHostIdentifier hostIdentifier;
    UInt32 size = sizeof(hostIdentifier);
    OSStatus status = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_AUHostIdentifier, kAudioUnitScope_Global, 0, &hostIdentifier, &size);
    
    if (status == noErr) {
        return CFStringToQString(hostIdentifier.hostName);
    }
   
    // trying a second approach
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle)
    {
        CFStringRef identifier = CFBundleGetIdentifier(mainBundle);
        QString identifierString = CFStringToQString(identifier); // format = com.org.<hostName>
        int index = identifierString.lastIndexOf(".");
        if (index > 0) {
            identifierString = identifierString.right(identifierString.size() - (index + 1));
        }
        identifierString = identifierString.at(0).toUpper() + identifierString.right(identifierString.size()-1);
        
        return identifierString;
    }

    return "Error!";
}
