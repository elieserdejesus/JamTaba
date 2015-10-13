#include "vst.h"
#include <cmath>
#include <QWinWidget>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <Windows.h>
#include <QStandardPaths>
#include "../MainController.h"
#include "../log/logHandler.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
JamtabaVstEditor::JamtabaVstEditor(JamtabaPlugin *jamtaba)
    :widget(NULL), jamtaba(jamtaba), mainWindow(nullptr)
{
}

JamtabaVstEditor::~JamtabaVstEditor()
{
    qWarning() << "JamtabaVstEditor destructor";
    if(mainWindow){
        qWarning() << "deleting main window";
        delete mainWindow;
        mainWindow = nullptr;
    }
    qWarning() << "JamtabaVstEditor destructor finished!";
}

void JamtabaVstEditor::detachMainController(){
    if(mainWindow){
        mainWindow->detachMainController();
    }
}

bool JamtabaVstEditor::getRect (ERect** rect)
{
    *rect = &rectangle;
    return true;
}

void JamtabaVstEditor::clientResize(HWND h_parent, int width, int height)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(h_parent, &rcClient);
    GetWindowRect(h_parent, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(h_parent, rcWindow.left, rcWindow.top, width + ptDiff.x, height + ptDiff.y, TRUE);
}

bool JamtabaVstEditor::open(void* ptr){

    if(!ptr )
        return false;

    if(!jamtaba->isRunning()){
        jamtaba->initialize();//initialize first time editor is opened
    }

    AEffEditor::open(ptr);
    widget = new QWinWidget(static_cast<HWND>(ptr));
    widget->setAutoFillBackground(false);
    widget->setObjectName("QWinWidget");

    if(!mainWindow){
        qWarning() << "Criando MainWindow";
        mainWindow = new MainWindow(jamtaba->getController());
        rectangle.left = 0;
        rectangle.top = 0;
        rectangle.right = mainWindow->width();
        rectangle.bottom = mainWindow->height();
    }
    mainWindow->setParent(widget);
    mainWindow->move( 0, 0 );

    rectangle.bottom = mainWindow->height();
    rectangle.right = mainWindow->width();

    widget->move( 0, 0 );
    widget->resize( rectangle.right, rectangle.bottom );
    widget->setMinimumSize(mainWindow->minimumSize());

    qDebug() << "Window Rect  width:" << rectangle.right - rectangle.left;

    widget->setPalette( mainWindow->palette() );

//    resizeH = new ResizeHandle(widget);
//    QPoint pos( widget->geometry().bottomRight() );
//    pos.rx()-=resizeH->width();
//    pos.ry()-=resizeH->height();
//    resizeH->move(pos);
//    resizeH->show();

//    connect(resizeH, SIGNAL(Moved(QPoint)),
//            this, SLOT(OnResizeHandleMove(QPoint)));

    widget->show();
    return true;
 }

void JamtabaVstEditor::close(){
    qWarning() << "JamtabaVstEditor::close()";
    if(mainWindow){
        mainWindow->setParent(nullptr);
    }
    if(widget){
        delete widget;
        widget = nullptr;
    }
}

//+++++++++++++
class AudioDriverVST : public Audio::AudioDriver{
public:
    AudioDriverVST(Audio::AudioDriverListener* listener)
        :AudioDriver(listener){

    }

    void stop(){}
    void start(){}
    void release(){}
    QList<int> getValidSampleRates(int deviceIndex) const{return QList<int>();}
    QList<int> getValidBufferSizes(int deviceIndex) const{return QList<int>();}
    int getMaxInputs() const{return 2;}
    int getMaxOutputs() const{return 2;}
    const char* getInputChannelName(const unsigned int index) const{return "";}
    const char* getOutputChannelName(const unsigned int index) const{return "";}
    const char* getInputDeviceName(int index) const{return "";}
    const char* getOutputDeviceName(int index) const{return "";}
    int getInputDeviceIndex() const{return -1;}
    int getOutputDeviceIndex() const{return -1;}
    void setInputDeviceIndex(int ){}
    void setOutputDeviceIndex(int){}
    int getDevicesCount() const{return 1;}
};

//+++++++++++++
class MainControllerVST : public Controller::MainController{
public:
    MainControllerVST(Persistence::Settings settings)
        :MainController(settings){
        qDebug() << "Creating MainControllerVST instance!";
    }

    Audio::AudioDriver* buildAudioDriver(const Persistence::Settings &settings){
        return new AudioDriverVST(nullptr);
    }

    void setCSS(QString css){
        qDebug() << "setting CSS";
        qApp->setStyleSheet(css);//qApp is a global variable created in dll main.
    }

    Vst::PluginFinder* createPluginFinder(){return nullptr;}

    Audio::Plugin* createPluginInstance(const Audio::PluginDescriptor &descriptor){
        Q_UNUSED(descriptor);
        return nullptr;//vst plugin can't load other plugins
    }


    Midi::MidiDriver* createMidiDriver(){return nullptr;}

    bool useMidiDriver(){return false;}

    void exit(){}

    void addDefaultPluginsScanPath(){}
    void scanPlugins(){}
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
    return new JamtabaPlugin (audioMaster);
}

JamtabaPlugin::JamtabaPlugin (audioMasterCallback audioMaster) :
    AudioEffectX (audioMaster, 0, 0),
    bufferSize(0),
    listEvnts(0),
    controller(nullptr),
    running(false)
{

    setNumInputs (DEFAULT_INPUTS*2);
    setNumOutputs (DEFAULT_OUTPUTS*2);

    isSynth(false);
    canProcessReplacing(true);
    programsAreChunks(false);
    vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name

    canDoubleReplacing(false);

    setEditor(new JamtabaVstEditor(this));
}

VstInt32 JamtabaPlugin::fxIdle(){
    QApplication::processEvents();
    return 0;
}

bool JamtabaPlugin::needIdle(){
    return true;
}

JamtabaPlugin::~JamtabaPlugin ()
{

}

void JamtabaPlugin::initialize(){
    if(!isRunning()){
        if(!controller){
            //qDebug() << "settings " << QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
            QApplication::setApplicationName("Jamtaba 2");
            QApplication::setApplicationVersion(APP_VERSION);

            qInstallMessageHandler(customLogHandler);
            Persistence::Settings settings;//read from file in constructor
            settings.load();
            qDebug()<< "Creating controller!";
            controller = new MainControllerVST(settings);
            controller->configureStyleSheet("jamtaba.css");
            controller->start();
            qDebug()<< "Controller started!";
            running = true;
        }
    }
}

void JamtabaPlugin::open(){


    //hostReportConnectionChanges = (bool)canHostDo((char*)"reportConnectionChanges");
    //hostAcceptIOChanges = (bool)canHostDo((char*)"acceptIOChanges");

    //reaper returns false :-/
//    char str[64];
//    getHostProductString(str);
//    if(!strcmp(str,"REAPER")) {
//        hostAcceptIOChanges = true;
//    }

//    opened=true;

}

void JamtabaPlugin::close()
{
    qWarning() << "JamtabaPlugin::close()";
    if(editor){
        qWarning() << "JamtabaPlugin::close() deleting editor";
        AEffEditor* theEditor = editor;
        setEditor(nullptr);
        delete theEditor;
        qWarning() << "JamtabaPlugin::close() editor deleted!";
    }
    if(controller){
        qWarning() << "JamtabaPlugin::close() deleting controller!";
        delete controller;
        controller = nullptr;
        qWarning() << "JamtabaPlugin::close() controller deleted!";
    }
    running = false;
}

VstInt32 JamtabaPlugin::getNumMidiInputChannels()
{
    return 0;
}

VstInt32 JamtabaPlugin::getNumMidiOutputChannels()
{
    return 0;
}

bool JamtabaPlugin::getEffectName (char* name)
{
    if(!name)
        return false;
    vst_strncpy (name, "Jamtaba 2", kVstMaxEffectNameLen);
    return true;
}

bool JamtabaPlugin::getProductString (char* text)
{
    if(!text)
        return false;
    vst_strncpy (text, "Jamtaba 2", kVstMaxProductStrLen);
    return true;
}

bool JamtabaPlugin::getVendorString (char* text)
{
    if(!text)
        return false;
    vst_strncpy (text, "Elieser de Jesus", kVstMaxVendorStrLen);
    return true;
}

VstInt32 JamtabaPlugin::getVendorVersion ()
{
    return 2;
}

VstInt32 JamtabaPlugin::canDo(char* text)
{
    if(!text)
        return 0;

    //     "offline", plug-in supports offline functions (offlineNotify, offlinePrepare, offlineRun)
    //     "midiProgramNames", plug-in supports function getMidiProgramName ()
    //     "bypass", plug-in supports function setBypass ()

    //    if (
    //            (!strcmp(text, "sendVstEvents")) ||
    //            (!strcmp(text, "sendVstMidiEvent")) ||
    //            (!strcmp(text, "receiveVstEvents")) ||
    //            (!strcmp(text, "receiveVstMidiEvent")) ||
    //            (!strcmp(text, "receiveVstTimeInfo"))
    //         )
    //         return 1;

    return 0;

}

//VstInt32 Vst::processEvents(VstEvents* events)
//{
//    if(!events || !hostSendVstEvents || !hostSendVstMidiEvent)
//        return 0;

//    VstEvent *evnt=0;

//    for(int i=0; i<events->numEvents; i++) {
//        evnt=events->events[i];
//        if( evnt->type==kVstMidiType) {
//            VstMidiEvent *midiEvnt = (VstMidiEvent*)evnt;

//            foreach(Connectables::VstMidiDevice *dev, lstMidiIn) {
//                long msg;
//                memcpy(&msg, midiEvnt->midiData, sizeof(midiEvnt->midiData));
//                dev->midiQueue << msg;
//            }
//        } else {
//            LOG("other vst event");
//        }
//    }

//    return 1;
//}

void JamtabaPlugin::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{

}


void JamtabaPlugin::setSampleRate(float sampleRate)
{

}

void JamtabaPlugin::setBlockSize (VstInt32 blockSize)
{
    if(blockSize<=0)
        return;
    bufferSize=blockSize;

}

void JamtabaPlugin::suspend()
{

}

void JamtabaPlugin::resume()
{

}
