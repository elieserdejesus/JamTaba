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
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QThread>
#include <cassert>

Q_LOGGING_CATEGORY(vst, "vst")

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
        //started(false)
{
    this->vstMidiEvents.reserved = 0;
    this->vstMidiEvents.numEvents = 0;
    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        this->vstMidiEvents.events[i] = (VstEvent*)(new VstMidiEvent);
    }
}

bool VstPlugin::load(VstHost *host, QString path){
    QString pluginDir = QFileInfo(path).absoluteDir().absolutePath();
    //qCDebug(vst) << "adding " << pluginDir << "in library path";
    QApplication::addLibraryPath(pluginDir);

    pluginLib.setFileName(path);
    effect = nullptr;

    vstPluginFuncPtr entryPoint=0;
    try {
        qCDebug(vst) << "loading VST plugin " << path << " thread:" << QThread::currentThreadId();
        if(!pluginLib.load()){
            qCCritical(vst) << "error when loading VST plugin " << path;
            return false;
        }
        //qCDebug(vst) << path << " loaded";
        //qCDebug(vst) << "searching VST plugin entry point for" << path;
        entryPoint = (vstPluginFuncPtr)pluginLib.resolve("VSTPluginMain");
        if(!entryPoint){
            entryPoint = (vstPluginFuncPtr)pluginLib.resolve("main");
        }
    }
    catch(...)
    {
        qCCritical(vst) << "exception when  getting entry point " << pluginLib.fileName();
    }
    if(!entryPoint) {
        qCCritical(vst) << "Entry point not founded, unloading plugin " << path ;
        unload();
        return false;
    }
    //qCDebug(vst) << "Entry point founded for " << path ;
    try
    {
        //qCDebug(vst) << "Initializing effect for " << path ;
        effect = entryPoint( host->hostCallback);// myHost->vstHost->AudioMasterCallback);
    }
    catch(...)
    {
        effect = nullptr;
    }

    if(!effect) {
        qCCritical(vst) << "Error when initializing effect. Unloading " << path ;
        unload();
        return false;
    }

    if (effect->magic != kEffectMagic) {
        qCCritical(vst) << "KEffectMagic error for " << path ;
        unload();
        return false;
    }
    char name[kVstMaxEffectNameLen];
    //qCDebug(vst) << "getting name for " << path ;
    effect->dispatcher(effect, effGetEffectName, 0, 0, name, 0);
    this->name = QString(name);
    //qCDebug(vst) << "Name readed: " << name ;
    this->path = path;

    int outputs = effect->numOutputs;
    qCDebug(vst) << "Criando internalBuffer com " << outputs << " canais e " << host->getBufferSize() << " samples";
    internalBuffer = new Audio::SamplesBuffer(outputs, host->getBufferSize());

    return true;
}


QByteArray VstPlugin::getSerializedData() const{
    if(effect->flags & effFlagsProgramChunks){//can serialize chunks?
        char* chunk = 0;
        long result = effect->dispatcher(effect, effGetChunk, false, 0, &chunk, 0 );
        if(result){
            qCDebug(vst) << "saving " << getName() << " state";
            return QByteArray(chunk, result);
        }
    }
    return QByteArray();//empty byte array
}

void VstPlugin::restoreFromSerializedData(QByteArray dataToRestore){
    qWarning() << "restoring plugin data";
    char* data = dataToRestore.data();
    effect->dispatcher(effect, effSetChunk, false, dataToRestore.size(), data, 0 );
}

void VstPlugin::resume(){
    qCDebug(vst) << "Resuming " << getName() << "thread: " << QThread::currentThreadId();
    effect->dispatcher(effect, effMainsChanged, 0, 1, NULL, 0.0f);
}

void VstPlugin::suspend(){
    qCDebug(vst) << "Suspending " << getName() << "Thread: " << QThread::currentThreadId();
    effect->dispatcher(effect, effMainsChanged, 0, 0, NULL, 0.0f);

    getSerializedData();
}


void VstPlugin::start(){
    if(!effect){
        qCCritical(vst) << "effect not set, returning!";
        return;
    }

    qCDebug(vst) << "starting plugin " << getName() << " thread: " << QThread::currentThreadId();

    //long ver = effect->dispatcher(effect, effGetVstVersion, 0, 0, NULL, 0);// EffGetVstVersion();
    //qDebug() << "Starting " << getName() << " version " << ver;


    //qCDebug(vst) << "opening" << getName();
    effect->dispatcher(effect, effOpen, 0, 0, NULL, 0.0f);
    //qCDebug(vst) << getName() << "opened";
    //qCDebug(vst) << "setting sample rate and block size";
    effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, host->getSampleRate());
    effect->dispatcher(effect, effSetBlockSize, 0, host->getBufferSize(), NULL, 0.0f);
    //qCDebug(vst) << "sample rate and block size setted for " << getName();

    //qCDebug(vst) << "checking for plugin midi capabilities";
    wantMidi = (effect->dispatcher(effect, effCanDo, 0, 0, (void*)"receiveVstMidiEvent", 0) == 1);
    //qCDebug(vst) << "plugin midi capabilities done: " << wantMidi;


    //read chunk to test
    QFile f("test.dat");
    if(!f.open(QFile::ReadOnly)){
        qCritical() << "Não conseguiu abrir o arquivo ";
    }

    resume();

    //effect->dispatcher(effect, effStartProcess, 0, 1, NULL, 0.0f);


    //started = true;
}

VstPlugin::~VstPlugin()
{
    qCDebug(vst) << getName() << " VSt plugin destructor ";
    unload();
    delete internalBuffer;

    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        delete this->vstMidiEvents.events[i];
    }
}

void VstPlugin::unload(){
    qCDebug(vst) << "unloading VST plugin " << getName();
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

void VstPlugin::fillVstEventsList(const Midi::MidiBuffer &midiBuffer){

    //qCDebug(vst) << "filling VST midi event list";

    int midiMessages = std::min( midiBuffer.getMessagesCount(), MAX_MIDI_EVENTS);
    this->vstMidiEvents.numEvents = midiMessages;
    for (int m = 0; m < midiMessages; ++m) {
        Midi::MidiMessage message = midiBuffer.getMessage(m);
        VstMidiEvent* vstEvent = (VstMidiEvent*)vstMidiEvents.events[m];
        vstEvent->type = kVstMidiType;
        vstEvent->byteSize = sizeof(vstEvent);
        vstEvent->deltaFrames = 0;
        vstEvent->midiData[0] = Pm_MessageStatus(message.data);
        vstEvent->midiData[1] = Pm_MessageData1(message.data);
        vstEvent->midiData[2] = Pm_MessageData2(message.data);
        vstEvent->midiData[3] = 0;
        vstEvent->reserved1 = vstEvent->reserved2 = 0;
        vstEvent->flags = kVstMidiEventIsRealtime;
    }
}

void VstPlugin::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &outBuffer, const Midi::MidiBuffer& midiBuffer){
    if(isBypassed() || !effect || !internalBuffer){
        qWarning() << "returning";
        return;
    }

    //qCDebug(vst) << "processing...";
    if(wantMidi){
        //const VstEvents* events = host->getVstMidiEvents();
        fillVstEventsList(midiBuffer);//translate midiBuffer messages in VstEvents
        //qCDebug(vst) << "sending effProcessEvents to plugin " + getName();
        effect->dispatcher(effect, effProcessEvents, 0, 0, (void*)&vstMidiEvents, 0);
    }

    internalBuffer->setFrameLenght(outBuffer.getFrameLenght());

    float* inArray[2] = {
        outBuffer.getSamplesArray(0),
        outBuffer.getSamplesArray(1)
    };
    //vst plugins maybe have many output channels
    int outChannels = internalBuffer->getChannels();
    float* out[outChannels];
    for (int c = 0; c < outChannels; ++c) {
       out[c] = internalBuffer->getSamplesArray(c);
    }

    VstInt32 sampleFrames = outBuffer.getFrameLenght();

    //qCDebug(vst) << "calling processReplacing in " << getName() << " thread" << QThread::currentThreadId();
    effect->processReplacing(effect, inArray, out, sampleFrames);
    //qCDebug(vst) << "processReplacing called " << getName();

    //mix multiple out plugins to stereo
    int totalChannels = internalBuffer->getChannels();
    if(totalChannels > 2){
        for (int s = 0; s < internalBuffer->getFrameLenght(); ++s) {
            for (int c = 2; c < totalChannels; ++c) {
                internalBuffer->add(c % 2, s, internalBuffer->get(c, s));
            }
        }
    }

    outBuffer.set(*internalBuffer);
    //qCDebug(vst) << "audioBuffer filled " << getName();
}

void VstPlugin::openEditor(QPoint centerOfScreen){
    if(!effect ){
        return;
    }

    if(!(effect->flags & effFlagsHasEditor || !hasEditorWindow())){
        return;
    }

    qCDebug(vst) << "opening " <<getName() << "editor thread: " << QThread::currentThreadId();

    //obtém o tamanho da janela do plugin
    ERect* rect;
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
    if (!rect) {
      qCCritical(vst) << "VST plugin returned NULL edit rect";
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

    qCDebug(vst) << getName() << " editor opened";

    //resume();
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
