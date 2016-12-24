#include "JamTabaAUPlugin.h"
#include <QMacNativeWidget>


JamTabaAUPlugin* JamTabaAUPlugin::instance = nullptr;


Listener::Listener(JamTabaAUPlugin *auPlugin)
    : auPlugin(auPlugin)
{
        
}
    
void Listener::process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess) 
{
   auPlugin->process(inBuffer, outBuffer, inFramesToProcess);
}
    
void Listener::cleanUp()
{
    JamTabaAUPlugin::releaseInstance();
}

////++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
        dynamic_cast<JamTabaAUPlugin*>(this->plugin)->resizeWindow(newWidth, newHeight);
    }
};


JamTabaAUPlugin::JamTabaAUPlugin(AudioUnit audioUnit)
    :JamTabaPlugin(2, 2),
    listener(new Listener(this)),
    audioUnit(audioUnit)
{
    
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
    qDebug() << "Initializing JamTabaAUPlugin";
    
    JamTabaPlugin::initialize();
 
    MainControllerPlugin *mainController = getController();
    mainWindow = new MainWindowPlugin(mainController);
    mainController->setMainWindow(mainWindow);
    mainWindow->initialize();
    
    nativeView = createNativeView();
}

void JamTabaAUPlugin::finalize()
{
    if (nativeView && mainWindow) {
        qDebug() << "Deleting windows";
        nativeView->deleteLater();
    
        nativeView = nullptr;
        mainWindow = nullptr;
    }
}

void JamTabaAUPlugin::resizeWindow(int newWidth, int newHeight)
{
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
        //[uiFreshlyLoadedView setFrame: frame];
    }
}

QMacNativeWidget *JamTabaAUPlugin::createNativeView()
{
    QMacNativeWidget *nativeWidget = new QMacNativeWidget();
    nativeWidget->setWindowFlags(Qt::Tool); // without this flag the plugin window is not showed in AULab and Logic 9
    
    nativeWidget->clearFocus();
    nativeWidget->releaseKeyboard();
    nativeWidget->setAttribute(Qt::WA_ShowWithoutActivating);
    nativeWidget->setAttribute(Qt::WA_NativeWindow);
    
    nativeWidget->nativeView();
    
    return nativeWidget;
}

void JamTabaAUPlugin::process(const AudioBufferList &inBuffer, AudioBufferList &outBuffer, UInt32 inFramesToProcess)
{
    if (inBuffer.mNumberBuffers != inputBuffer.getChannels())
        return;
    
    if (!controller)
        return;
    
    if (controller->isPlayingInNinjamRoom()) {
        
        // ++++++++++ sync ninjam BPM with host BPM ++++++++++++
        // ask timeInfo to VST host
        
//        timeInfo = getTimeInfo(kVstTransportPlaying | kVstTransportChanged | kVstTempoValid);
//        if (transportStartDetectedInHost()) {// user pressing play/start in host?
//            NinjamControllerPlugin *ninjamController = controller->getNinjamController();
//            Q_ASSERT(ninjamController);
//            if (ninjamController->isWaitingForHostSync())
//                ninjamController->startSynchronizedWithHost(getStartPositionForHostSync());
//        }
    }
    
    // ++++++++++ Audio processing +++++++++++++++
    inputBuffer.setFrameLenght(inFramesToProcess);
    for (int c = 0; c < inputBuffer.getChannels(); ++c)
        memcpy(inputBuffer.getSamplesArray(c), inBuffer.mBuffers[c].mData, sizeof(float) * inFramesToProcess);
    
    outputBuffer.setFrameLenght(inFramesToProcess);
    outputBuffer.zero();
    
    controller->process(inputBuffer, outputBuffer, getSampleRate());
    
    int channels = outputBuffer.getChannels();
    for (int c = 0; c < channels; ++c)
        memcpy(outBuffer.mBuffers[c].mData, outputBuffer.getSamplesArray(c), sizeof(float) * inFramesToProcess);
    
    // ++++++++++++++++++++++++++++++
    hostWasPlayingInLastAudioCallBack = hostIsPlaying();
}

MainControllerPlugin * JamTabaAUPlugin::createPluginMainController(const Persistence::Settings &settings, JamTabaPlugin *plugin) const
{
    return new MainControllerAU(settings, plugin);
}

qint32 JamTabaAUPlugin::getStartPositionForHostSync() const
{
    return 0; // TODO implementar
}

bool JamTabaAUPlugin::hostIsPlaying() const
{
    return true; //TODO implementar
}

int JamTabaAUPlugin::getHostBpm() const
{
    int bpm = 0;
    UInt32 size = sizeof(int *);
    AudioUnitGetProperty(audioUnit, kJamTabaGetHostBPM, kAudioUnitScope_Global, 0, &bpm, &size);
    return bpm;
}

float JamTabaAUPlugin::getSampleRate() const
{
    return 44100; // TODO implementar
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
