#include "vst.h"
#include <cmath>
#include <QWinWidget>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <Windows.h>
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

JamtabaWindow::JamtabaWindow(Jamtaba *jamtaba, HWND parent)
    :QWinWidget(parent, NULL), jamtaba(jamtaba), h_parent(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QLabel *label = new QLabel("Gain", this);
    valueLabel = new QLabel("0", this);
    slider = new QSlider(Qt::Horizontal, this);
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(valueLabel);
    layout->addWidget(slider);
    setLayout(layout);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(on_sliderChanged(int)));
}

void JamtabaWindow::on_sliderChanged(int value){
    jamtaba->setGain((float)value/slider->maximum());
    valueLabel->setText(QString::number(value));
}




JamtabaEditor::JamtabaEditor(Jamtaba* jamtaba)
    :widget(NULL), jamtaba(jamtaba)
{
}

JamtabaEditor::~JamtabaEditor()
{
}

bool JamtabaEditor::getRect (ERect** rect)
{
    *rect = &rectangle;
    return true;
}

void JamtabaEditor::clientResize(HWND h_parent, int width, int height)
{
    RECT rcClient, rcWindow;
    POINT ptDiff;
    GetClientRect(h_parent, &rcClient);
    GetWindowRect(h_parent, &rcWindow);
    ptDiff.x = (rcWindow.right - rcWindow.left) - rcClient.right;
    ptDiff.y = (rcWindow.bottom - rcWindow.top) - rcClient.bottom;
    MoveWindow(h_parent, rcWindow.left, rcWindow.top, width + ptDiff.x, height + ptDiff.y, TRUE);
}

bool JamtabaEditor::open(void* ptr){
    AEffEditor::open (ptr);
    widget = new JamtabaWindow(jamtaba, static_cast<HWND>(ptr));
    widget->move( 0, 0 );
    widget->adjustSize();
    rectangle.top = 0;
    rectangle.left = 0;
    rectangle.bottom = widget->height();
    rectangle.right = widget->width();
    widget->setMinimumSize(widget->size());
    widget->show();
    clientResize(static_cast<HWND>(ptr), widget->width(), widget->height());


    return true;
}

void JamtabaEditor::close(){
    delete widget;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
    return new Jamtaba (audioMaster);
}

Jamtaba::Jamtaba (audioMasterCallback audioMaster) :
    AudioEffectX (audioMaster, 0, 0),
    bufferSize(0),
    listEvnts(0),
    hostSendVstEvents(false),
    hostSendVstMidiEvent(false),
    hostReportConnectionChanges(false),
    hostAcceptIOChanges(false),
    hostSendVstTimeInfo(false),
    hostReceiveVstEvents(false),
    hostReceiveVstMidiEvents(false),
    hostReceiveVstTimeInfo(false),
    opened(false)
  //editor(nullptr)

{

    this->gain = 1;

    setNumInputs (DEFAULT_INPUTS*2);
    setNumOutputs (DEFAULT_OUTPUTS*2);

    isSynth(false);
    canProcessReplacing(true);
    programsAreChunks(false);
    vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name

    //QCoreApplication::setOrganizationName("www.jamtaba.com");
    //QCoreApplication::setApplicationName("Jamtaba 2");



    canDoubleReplacing(false);

    //this->editor = new QVstGui();
    setEditor(new JamtabaEditor(this));

    //qEditor = new Gui(this);
    //setEditor(qEditor);
    //qEditor->SetMainWindow(myWindow);
}

VstInt32 Jamtaba::fxIdle(){
    //QApplication::processEvents();
    return 0;
}

bool Jamtaba::needIdle(){
    return true;
}

Jamtaba::~Jamtaba ()
{

}


void Jamtaba::open()
{
    //     char str[64];
    //     getHostVendorString(str);
    //     getHostProductString(str);
    //     VstInt32 hostVer = getHostVendorVersion();

    //hostSendVstEvents = (bool)canHostDo((char*)"sendVstEvents");
    //hostSendVstMidiEvent = (bool)canHostDo((char*)"sendVstMidiEvent");
    //hostSendVstTimeInfo = (bool)canHostDo((char*)"sendVstTimeInfo");
    //hostReceiveVstEvents = (bool)canHostDo((char*)"receiveVstEvents");
    //hostReceiveVstMidiEvents = (bool)canHostDo((char*)"receiveVstMidiEvents");
    //hostReceiveVstTimeInfo = (bool)canHostDo((char*)"receiveVstTimeInfo");
    //hostReportConnectionChanges = (bool)canHostDo((char*)"reportConnectionChanges");
    //hostAcceptIOChanges = (bool)canHostDo((char*)"acceptIOChanges");

    //reaper returns false :-/
    char str[64];
    getHostProductString(str);
    if(!strcmp(str,"REAPER")) {
        hostAcceptIOChanges = true;
    }

    opened=true;

}

void Jamtaba::close()
{

    opened=false;
}

VstInt32 Jamtaba::getNumMidiInputChannels()
{
    return 15;
}

VstInt32 Jamtaba::getNumMidiOutputChannels()
{
    return 15;
}

bool Jamtaba::getEffectName (char* name)
{
    if(!name)
        return false;
    vst_strncpy (name, "Jamtaba 2", kVstMaxEffectNameLen);
    return true;
}

bool Jamtaba::getProductString (char* text)
{
    if(!text)
        return false;
    vst_strncpy (text, "Jamtaba 2", kVstMaxProductStrLen);
    return true;
}

bool Jamtaba::getVendorString (char* text)
{
    if(!text)
        return false;
    vst_strncpy (text, "Elieser de Jesus", kVstMaxVendorStrLen);
    return true;
}

VstInt32 Jamtaba::getVendorVersion ()
{
    return 2;
}

VstInt32 Jamtaba::canDo(char* text)
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

float angulo = 0;
float phaseIncrement = 2 * 3.1415 / 44100 * 440;

void Jamtaba::processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames)
{
    if(!inputs || !outputs || sampleFrames<=0)
        return;

    //int inputs = DEFAULT_INPUTS * 2;
    //int outputChannels = DEFAULT_OUTPUTS * 2;
    for (int s = 0; s < sampleFrames; ++s) {
        outputs[0][s] = std::sin(angulo) * this->gain;
        angulo += phaseIncrement;
    }
}


void Jamtaba::setSampleRate(float sampleRate)
{

}

void Jamtaba::setBlockSize (VstInt32 blockSize)
{
    if(blockSize<=0)
        return;
    bufferSize=blockSize;

}

void Jamtaba::suspend()
{

}

void Jamtaba::resume()
{

}
