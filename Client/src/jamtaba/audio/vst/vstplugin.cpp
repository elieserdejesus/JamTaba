#include "VstPlugin.h"
#include "aeffectx.h"
#include "vsthost.h"
#include <windows.h>
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include "../audio/core/SamplesBuffer.h"
#include "../midi/MidiDriver.h"
#include "portmidi.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDesktopWidget>
#include <QLibrary>
#include <string>
#include <locale>
#include <QThread>


using namespace Vst;

//+++++++++++++++++++++++++++

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VstPlugin::VstPlugin(VstHost* host)
    :   Audio::Plugin("name"),
        effect(nullptr),
        internalBuffer(nullptr),
        host(host)
{

}

bool VstPlugin::load(VstHost *host, QString path){
    qWarning() << "Carregando plugin " << path << " thread: " << QThread::currentThreadId();
    pluginLib.setFileName(path);
    effect = nullptr;

    if(!pluginLib.load()){
        qCritical() << "error when loading VST plugin " << path;
        return false;
    }

    vstPluginFuncPtr entryPoint=0;
    try {
        entryPoint = (vstPluginFuncPtr)pluginLib.resolve("VSTPluginMain");
        if(!entryPoint){
            entryPoint = (vstPluginFuncPtr)pluginLib.resolve("main");
        }
    }
    catch(...)
    {
        qCritical() << "exception when  getting entry point " << pluginLib.fileName();
    }
    if(!entryPoint) {
        unload();
        return false;
    }

    try
    {
        effect = entryPoint( host->hostCallback);// myHost->vstHost->AudioMasterCallback);
    }
    catch(...)
    {
        effect = nullptr;
    }

    if(!effect) {
        unload();
        return false;
    }

    if (effect->magic != kEffectMagic) {
        unload();
        return false;
    }
    char name[kVstMaxEffectNameLen];
    effect->dispatcher(effect, effGetEffectName, 0, 0, name, 0);
    this->name = QString(name);


    this->path = path;

    return true;
}


void VstPlugin::resume(){
    effect->dispatcher(effect, effMainsChanged, 0, 1, NULL, 0.0f);
}

void VstPlugin::suspend(){
    effect->dispatcher(effect, effMainsChanged, 0, 0, NULL, 0.0f);
}


void VstPlugin::start(int sampleRate, int bufferSize){
    if(!effect){
        return;
    }

    qWarning() << "inicializando plugin " << getName() << " thread: " << QThread::currentThreadId();

    //long ver = effect->dispatcher(effect, effGetVstVersion, 0, 0, NULL, 0);// EffGetVstVersion();
    //qDebug() << "Starting " << getName() << " version " << ver;
    internalBuffer = new Audio::SamplesBuffer(effect->numOutputs, host->getBufferSize());

    effect->dispatcher(effect, effOpen, 0, 0, NULL, 0.0f);
    effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, sampleRate);
    effect->dispatcher(effect, effSetBlockSize, 0, bufferSize, NULL, 0.0f);

    wantMidi = (effect->dispatcher(effect, effCanDo, 0, 0, (void*)"receiveVstMidiEvent", 0) == 1);

    resume();
}

VstPlugin::~VstPlugin()
{
    qWarning() << getName() << " VSt plugin destructor ";
    unload();
    delete internalBuffer;
}

void VstPlugin::unload(){

    if(effect){
        effect->dispatcher(effect, effEditClose, 0, 0, NULL, 0);//fecha o editor
        suspend();
        effect->dispatcher(effect, effClose, 0, 0, NULL, 0);
        //delete effect;
        effect = nullptr;
    }
    if(pluginLib.isLoaded()){
        pluginLib.unload();
    }
}

void VstPlugin::process(Audio::SamplesBuffer &buffer){
    if(isBypassed() || !effect || !internalBuffer){
        return;
    }

    if(wantMidi){
        const VstEvents* events = host->getVstMidiEvents();
        effect->dispatcher(effect, effProcessEvents, 0, 0, (void*)events, 0);
    }

    internalBuffer->setFrameLenght(buffer.getFrameLenght());

    float* in[2] = {
        buffer.getSamplesArray(0),
        buffer.getSamplesArray(1)
    };
     float* out[2] = {
        internalBuffer->getSamplesArray(0),
        internalBuffer->getSamplesArray(1)
    };
    VstInt32 sampleFrames = buffer.getFrameLenght();
    effect->processReplacing(effect, in, out, sampleFrames);
    buffer.add(*internalBuffer);
}
/*
// C callbacks
extern "C" {
// Main host callback
  VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode,
    VstInt32 index, VstInt32 value, void *ptr, float opt){
        //qDebug() << "opcode:" <<opcode << "index:" <<index << "value:" <<value  ;
        if(opcode == audioMasterGetCurrentProcessLevel){
            return 1;
        }
        if(opcode == audioMasterVersion){
            return 2400;// VST 2.4
        }
        if(opcode == audioMasterGetProductString || opcode == audioMasterGetVendorString){
            ptr = (void*)"Jamtaba";
            return 1;
        }

        if(opcode == audioMasterIOChanged){
            return 1;//
        }
    }
}
*/
void VstPlugin::openEditor(QPoint centerOfScreen){
    if(!effect ){
        return;
    }
    if(!(effect->flags & effFlagsHasEditor || !hasEditorWindow())){
        return;
    }
    //suspend();
    //abre o editor do plugin usando o handle do diálogo
    //effect->dispatcher(effect, effEditOpen, 0, 0, (void*)(w->effectiveWinId()), 0);

    //obtém o tamanho da janela do plugin
    ERect* rect;
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
    if (!rect) {
      qDebug("VST plugin returned NULL edit rect");
      return;
    }
    int rectWidth = rect->right - rect->left;
    int rectHeight = rect->bottom - rect->top;
    Audio::PluginWindow* w = getEditor();
    w->setFixedSize(rectWidth, rectHeight);
    effect->dispatcher(effect, effEditOpen, 0, 0, (void*)(w->effectiveWinId()), 0);

    //Some plugins don't return the real size until after effEditOpen
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
    if (rect) {
      w->setFixedSize(rect->right - rect->left, rect->bottom - rect->top);
    }

    rectWidth = rect->right - rect->left;
    rectHeight = rect->bottom - rect->top;

    w->move(centerOfScreen.x() - rectWidth/2, centerOfScreen.y() - rectHeight/2);

    resume();
}
/*
bool VstPlugin::initPlugin()
{
    {
        QMutexLocker lock(&objMutex);

        long ver = effect->dispatcher(effect, effGetVstVersion);// EffGetVstVersion();

        //bufferSize = host->getBufferSize();
        //sampleRate = host->getSampleRate();

        //if(!(effect->flags & effFlagsCanDoubleReplacing))
        //    doublePrecision=false;

        EffSetSampleRate(sampleRate);
        EffSetBlockSize(bufferSize);
        EffOpen();
        EffSetSampleRate(sampleRate);
        EffSetBlockSize(bufferSize);

        //long canSndMidiEvnt = EffCanDo(PlugCanDos::canDoSendVstMidiEvent);
        bWantMidi = (EffCanDo("receiveVstMidiEvent") == 1);
//LOG("sendtime"<<EffCanDo("sendVstTimeInfo"));

     //   long midiPrgNames = EffCanDo("midiProgramNames");
        VstPinProperties pinProp;
        EffGetInputProperties(0,&pinProp);

        //stereo input
        bool stereoIn = false;
        if(pinProp.flags & kVstPinIsStereo)
                stereoIn = true;
        //speaker arrangement
        //pinProp->arrangementType

        EffGetOutputProperties(0,&pinProp);
        //stereo output
        bool stereoOut = false;
        if(pinProp.flags & kVstPinIsStereo)
                stereoOut = true;

        //speaker arrangement
        //pinProp->arrangementType

        EffResume();
        EffSuspend();

        if(stereoIn)
        {
                EffGetInputProperties(0,&pinProp);
                EffGetInputProperties(1,&pinProp);
        }
        if(stereoOut)
        {
                EffGetOutputProperties(0,&pinProp);
                EffGetOutputProperties(1,&pinProp);
        }

        if(bWantMidi)
        {
                EffGetNumMidiInputChannels();
                EffGetNumMidiOutputChannels();
        }

        if(ver>=2000 && ver<2400)
        {
                EffConnectInput(0,1);
                if(stereoIn)
                        EffConnectInput(1,1);

                EffConnectOutput(0,1);
                if(stereoOut)
                        EffConnectOutput(1,1);
        }

        EffSetProgram(0);

        char szBuf[256] = "";
        if ((EffGetProductString(szBuf)) && (*szBuf)) {
            setObjectName( QString("%1%2").arg(szBuf).arg(index) );
        } else {
            sName = sName.section("/",-1);
            sName = sName.section(".",0,-2);
            setObjectName( sName % QString::number(index) );
        }
        objInfo.name=objectName();

        if(bWantMidi) {
            listMidiPinIn->AddPin(0);
            listMidiPinOut->AddPin(0);
        }

        SetInitDelay(pEffect->initialDelay);
    }

    //create all parameters pins
    int nbParam = pEffect->numParams;
    for(int i=0;i<nbParam;i++) {
        listParameterPinIn->AddPin(i);
    }

    if(pEffect->flags & effFlagsHasEditor) {
        //editor pin
//        listEditorVisible << "close";
        listEditorVisible << "hide";
        listEditorVisible << "show";
        listParameterPinIn->AddPin(FixedPinNumber::editorVisible);

        //learning pin
        listIsLearning << "off";
        listIsLearning << "learn";
        listIsLearning << "unlearn";
        listParameterPinIn->AddPin(FixedPinNumber::learningMode);
    }
    EffSetProgram(0);
    Object::Open();

    if(myHost->GetSetting("fastEditorsOpenClose",true).toBool()) {
        CreateEditorWindow();
    }
    AddPluginToDatabase();

    if(!bankToLoad.isEmpty()) {
        QTimer::singleShot(0,this,SLOT(LoadBank()));
    }
    return true;
}
*/

/*
// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

//// Plugin's dispatcher function
typedef VstIntPtr (*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode,
  VstInt32 index, VstInt32 value, void *ptr, float opt);
*/
//+++++++++++++++++++++++++++++++++++++

//Vst::VstPlugin* Vst::load(QString pluginPath){
//    AEffect *plugin = nullptr;

//    QLibrary* pluginLib = new QLibrary(pluginPath);
//    if(!pluginLib->load()){
//        qCritical() << "não foi possível carregar " << pluginPath;
//        return nullptr;
//    }

//    vstPluginFuncPtr entryPoint=0;
//    try {
//        entryPoint = (vstPluginFuncPtr)pluginLib->resolve("VSTPluginMain");
//        if(!entryPoint)
//            entryPoint = (vstPluginFuncPtr)pluginLib->resolve("main");
//    }
//    catch(...)
//    {
//        qCritical() << "exception when  getting entry point";
//    }
//    if(!entryPoint) {
//        pluginLib->unload();
//        return nullptr;
//    }

//    try
//    {
//        plugin = entryPoint(hostCallback);// myHost->vstHost->AudioMasterCallback);
//    }
//    catch(...)
//    {
//        plugin = nullptr;
//    }

//    if(!plugin) {
//        pluginLib->unload();
//        return false;
//    }

//    if (plugin->magic != kEffectMagic) {
//        pluginLib->unload();
//        delete plugin;
//        return false;
//    }

//    char name[kVstMaxEffectNameLen];
//    vstPluginFuncPtr dispatcher = (vstPluginFuncPtr)plugin->dispatcher;
//    dispatcher(plugin, effGetEffectName, 0, 0, name, 0);

    //void startPlugin(AEffect *plugin) {
//      dispatcher(plugin, effOpen, 0, 0, NULL, 0.0f);

//      // Set some default properties
//      float sampleRate = 44100.0f;
//      dispatcher(plugin, effSetSampleRate, 0, 0, NULL, sampleRate);
//      int blocksize = 2048;
//      dispatcher(plugin, effSetBlockSize, 0, blocksize, NULL, 0.0f);

//      //void resumePlugin(AEffect *plugin) {
//        dispatcher(plugin, effMainsChanged, 0, 1, NULL, 0.0f);

//        //virtual long EffEditOpen(void *ptr) { long l = EffDispatch(effEditOpen, 0, 0, ptr); /* if (l > 0) */ bEditOpen = true; return l; }

//      //}

//      //void suspendPlugin(AEffect *plugin) {
////        dispatcher(plugin, effMainsChanged, 0, 0, NULL, 0.0f);
////      }

//      //resume();
//    //}




////    return new Vst::VstPlugin(QString(name), pluginPath, plugin);
////}
