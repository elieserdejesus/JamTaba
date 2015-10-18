#include "Plugin.h"
#include <cmath>
#include <QWinWidget>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QStandardPaths>
#include "MainControllerVST.h"
#include "../log/logHandler.h"
#include "Editor.h"

Q_LOGGING_CATEGORY(pluginVst, "plugin.vst")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AudioEffect* createEffectInstance (audioMasterCallback audioMaster)
{
    return new JamtabaPlugin (audioMaster);
}

JamtabaPlugin::JamtabaPlugin (audioMasterCallback audioMaster) :
    AudioEffectX (audioMaster, 0, 0),
    listEvnts(0),
    controller(nullptr),
    running(false),
    inputBuffer( DEFAULT_INPUTS*2),
    outputBuffer(DEFAULT_OUTPUTS*2)
{
    qCDebug(pluginVst) << "Plugin constructor...";
    setNumInputs (DEFAULT_INPUTS*2);
    setNumOutputs (DEFAULT_OUTPUTS*2);

    isSynth(false);
    canProcessReplacing(true);
    programsAreChunks(false);
    vst_strncpy (programName, "Default", kVstMaxProgNameLen);	// default program name

    canDoubleReplacing(false);

    setEditor(new VstEditor(this));

    //suspend();
    qCDebug(pluginVst) << "Plugin constructor done.";
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
    qCDebug(pluginVst) << "Plugin destructor";
}

void JamtabaPlugin::initialize(){
    if(!isRunning()){
        if(!controller){
            qCDebug(pluginVst) << "Plugin initialize()...";
            QApplication::setApplicationName("Jamtaba 2");
            QApplication::setApplicationVersion(APP_VERSION);

            qInstallMessageHandler(customLogHandler);
            Persistence::Settings settings;//read from file in constructor
            settings.load();
            qCDebug(pluginVst)<< "Creating controller!";
            controller.reset( new MainControllerVST(settings));
            controller->configureStyleSheet("jamtaba.css");
            controller->start();
            qCDebug(pluginVst)<< "Controller started!";
            running = true;
            qCDebug(pluginVst) << "Plugin initialize() done";
        }
    }
}

void JamtabaPlugin::open(){
    qCDebug(pluginVst) << "Plugin open()";

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
    qCDebug(pluginVst) << "JamtabaPlugin::close()";
    if(editor){
        if(editor->isOpen()){
            editor->close();
        }
        //qCDebug(pluginVst) << "deleting editor...";
        ((VstEditor*)editor)->deleteMainWindow();
        //AEffEditor* theEditor = editor;
        //setEditor(nullptr);
        //delete theEditor;
        //qCDebug(pluginVst) << "deleting editor done!";
    }
//    if(controller){
//        qCDebug(pluginVst) << "deleting controller...";
//        delete controller;
//        controller = nullptr;
//        qCDebug(pluginVst) << "deleting controller done!";
//    }
    running = false;
    //AudioEffectX::close();
    qCDebug(pluginVst) << "JamtabaPLugin::close() finished";
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
    vst_strncpy (text, "", kVstMaxVendorStrLen);
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
    if(!controller){
        return;
    }
    inputBuffer.setFrameLenght(sampleFrames);
    for (int c = 0; c < inputBuffer.getChannels(); ++c) {
        memcpy(inputBuffer.getSamplesArray(c), inputs[c], sizeof(float) * sampleFrames);
    }

    outputBuffer.setFrameLenght(sampleFrames);
    outputBuffer.zero();
    controller->process(inputBuffer, outputBuffer, this->sampleRate);

    int channels = outputBuffer.getChannels();
    for (int c = 0; c < channels ; ++c) {
        memcpy(outputs[c], outputBuffer.getSamplesArray(c), sizeof(float) * sampleFrames);
    }

}


void JamtabaPlugin::setSampleRate(float sampleRate){
    qCDebug(pluginVst) << "JamtabaPlugin::setSampleRate()";
    this->sampleRate = sampleRate;

}


void JamtabaPlugin::suspend()
{
    qCDebug(pluginVst) << "JamtabaPLugin::suspend()";
}

void JamtabaPlugin::resume()
{
    qCDebug(pluginVst) << "JamtabaPLugin::resume()";
}
