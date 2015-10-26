#include "VstPlugin.h"
#include "aeffectx.h"
#include "vsthost.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <excpt.h>
#endif

#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include "../audio/core/SamplesBuffer.h"
#include "../midi/MidiDriver.h"
//#include "portmidi.h"
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
#include <QDebug>
#include <cassert>

Q_LOGGING_CATEGORY(vst, "vst")

using namespace Vst;

//+++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VstPlugin::VstPlugin(Vst::Host* host)
    :   Audio::Plugin("name"),
        effect(nullptr),
        internalOutputBuffer(nullptr),
        host(host),
        loaded(false),
        started(false),
        turnedOn(false),
        vstOutputArray(nullptr),
        vstInputArray(nullptr)
{
    this->vstMidiEvents.reserved = 0;
    this->vstMidiEvents.numEvents = 0;
    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        this->vstMidiEvents.events[i] = (VstEvent*)(new VstMidiEvent);
    }

    assert(host);

}


bool VstPlugin::load(QString path){
    if(!host){
        return false;
    }
    loaded = false;
    QString pluginDir = QFileInfo(path).absoluteDir().absolutePath();
    //qCDebug(vst) << "adding " << pluginDir << "in library path";
    QApplication::addLibraryPath(pluginDir);

    pluginLib.setFileName(path);
    effect = nullptr;

    vstPluginFuncPtr entryPoint=0;

    try {
        qCDebug(vst) << "loading " << path << " thread:" << QThread::currentThreadId();
        if(!pluginLib.load()){
            qCCritical(vst) << "error when loading VST plugin " << path << " -> " << pluginLib.errorString();
            return false;
        }
        //qCDebug(vst) << path << " loaded";
        //qCDebug(vst) << "searching VST plugin entry point for" << path;
        entryPoint = (vstPluginFuncPtr) pluginLib.resolve("VSTPluginMain");
        if(!entryPoint){
            entryPoint = (vstPluginFuncPtr)pluginLib.resolve("main");
        }
    }
    catch(...){
        qCCritical(vst) << "exception when  getting entry point " << pluginLib.fileName();
    }
    if(!entryPoint) {
        qCDebug(vst) << "Entry point not founded, unloading plugin " << path ;
        unload();
        return false;
    }
    qCDebug(vst) << "Entry point founded for " << path ;
    QApplication::processEvents();
    try{
        qCDebug(vst) << "Initializing effect for " << path ;
        effect = entryPoint( (audioMasterCallback)host->hostCallback);// myHost->vstHost->AudioMasterCallback);
    }
    catch(... ){
        qCCritical(vst) << "ERRO carregando plugin VST";
        effect = nullptr;
    }

    if(!effect) {
        qCCritical(vst) << "Error when initializing effect. Unloading " << path ;
        unload();
        return false;
    }
    QApplication::processEvents();
    if (effect->magic != kEffectMagic) {
        qCCritical(vst) << "KEffectMagic error for " << path ;
        unload();
        return false;
    }
    char temp[128];//kVstMaxEffectNameLen]; //some dumb plugins don't respect
    //qCDebug(vst) << "getting name for " << path ;
    effect->dispatcher(effect, effGetEffectName, 0, 0, temp, 0);
    this->name = QString(temp);

    if(!this->name.at(0).isLetterOrNumber()){//some plugins (it's rare) are returning trash in effGetEffectName, but returning a nice string in effGetProductString
        effect->dispatcher(effect, effGetProductString, 0, 0, temp, 0);
        this->name = QString(temp);
    }

    long ver = effect->dispatcher(effect, effGetVstVersion, 0, 0, NULL, 0);// EffGetVstVersion();
    qCDebug(vst) << "loading " << getName() << " version " << ver;

    this->path = path;

    loaded = true;

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
    if(!dataToRestore.isEmpty()){
        qCDebug(vst) << "restoring plugin data";
        char* data = dataToRestore.data();
        effect->dispatcher(effect, effSetChunk, false, dataToRestore.size(), data, 0 );
        qCDebug(vst) << "restore plugin data finished";
    }
}

void VstPlugin::resume(){

    qCDebug(vst) << "Resuming " << getName() << "thread: " << QThread::currentThreadId();
    effect->dispatcher(effect, effMainsChanged, 0, 1, NULL, 0.0f);

    effect->dispatcher(effect, effStartProcess, 0, 1, NULL, 0.0f);

}

void VstPlugin::suspend(){
    qCDebug(vst) << "Suspending " << getName() << "Thread: " << QThread::currentThreadId();

    effect->dispatcher(effect, effStopProcess, 0, 1, NULL, 0.0f);

    effect->dispatcher(effect, effMainsChanged, 0, 0, NULL, 0.0f);
}


void VstPlugin::start(){
    if(!effect){
        qCCritical(vst) << "effect not set, returning!";
        return;
    }

    qCDebug(vst) << "starting plugin " << getName() << " thread: " << QThread::currentThreadId();

    int outputs = effect->numOutputs;
    int inputs = effect->numInputs;
    qCDebug(vst) << "Criando internalBuffer com " << outputs << " canais e " << host->getBufferSize() << " samples";

    //qWarning() << getName() << " ins:" << effect->numInputs << " outs:" << effect->numOutputs;
    internalOutputBuffer = new Audio::SamplesBuffer(outputs, host->getBufferSize());
    internalInputBuffer  = new Audio::SamplesBuffer(inputs, host->getBufferSize());

    vstOutputArray = new float*[outputs];
    vstInputArray = new float*[inputs];


    long ver = effect->dispatcher(effect, effGetVstVersion, 0, 0, NULL, 0);// EffGetVstVersion();
    qCDebug(vst) << "Starting " << getName() << " version " << ver;

    //setting buffer size and sample before open just to avoid problems (I see this trick in VstBoard source code)
    qCDebug(vst) << "setting sample rate and block size " << QThread::currentThreadId();
    effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, host->getSampleRate());
    effect->dispatcher(effect, effSetBlockSize, 0, host->getBufferSize(), NULL, 0.0f);

    //qCDebug(vst) << "opening" << getName();
    effect->dispatcher(effect, effOpen, 0, 0, NULL, 0.0f);
    //qCDebug(vst) << getName() << "opened";

    qCDebug(vst) << "setting sample rate and block size " << QThread::currentThreadId();
    effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, host->getSampleRate());
    effect->dispatcher(effect, effSetBlockSize, 0, host->getBufferSize(), NULL, 0.0f);
    //qCDebug(vst) << "sample rate and block size setted for " << getName();



    //qCDebug(vst) << "checking for plugin midi capabilities";
    wantMidi = (effect->dispatcher(effect, effCanDo, 0, 0, (void*)"receiveVstMidiEvent", 0) == 1);
    //qCDebug(vst) << "plugin midi capabilities done: " << wantMidi;

    started = true;
    turnedOn = false;

    //I see this in VstBoard sources, and this save my life solving a bug in EzDrummer. Some Vst Plugins
    //do initialization in first resume?
    resume();
    suspend();
}

void VstPlugin::setSampleRate(int newSampleRate){
    if(effect){
        effect->dispatcher(effect, effSetSampleRate, 0, 0, NULL, newSampleRate);
    }
}

VstPlugin::~VstPlugin(){
    qCDebug(vst) << getName() << " VSt plugin destructor Thread:" << QThread::currentThreadId();
    unload();
    if(editorWindow){
        editorWindow->deleteLater();
        editorWindow = nullptr;
    }
    delete internalOutputBuffer;

    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        delete this->vstMidiEvents.events[i];
    }

    if(vstOutputArray){
        delete [] vstOutputArray;
    }
    if(vstInputArray){
        delete [] vstInputArray;
    }
}

void VstPlugin::unload(){
    qCDebug(vst) << "unloading VST plugin " << getName() << " Thread:" << QThread::currentThreadId();
    if(effect){

        closeEditor();

        suspend();

        if(effect){
            effect->dispatcher(effect, effClose, 0, 0, NULL, 0);

            effect = nullptr;
        }
    }
    if( pluginLib.isLoaded()){
        pluginLib.unload();
    }
}

void VstPlugin::fillVstEventsList(const Midi::MidiBuffer &midiBuffer){
    int midiMessages = qMin( midiBuffer.getMessagesCount(), MAX_MIDI_EVENTS);
    this->vstMidiEvents.numEvents = midiMessages;
    for (int m = 0; m < midiMessages; ++m) {
        Midi::MidiMessage message = midiBuffer.getMessage(m);
        qCDebug(vst) << getName() << "Midi message: channel " << message.getChannel();
        VstMidiEvent* vstEvent = (VstMidiEvent*)vstMidiEvents.events[m];
        vstEvent->type = kVstMidiType;
        vstEvent->byteSize = sizeof(vstEvent);
        vstEvent->deltaFrames = 0;
        vstEvent->midiData[0] = message.getStatus();
        vstEvent->midiData[1] = message.getData1();
        vstEvent->midiData[2] = message.getData2();
        vstEvent->midiData[3] = 0;
        vstEvent->reserved1 = vstEvent->reserved2 = 0;
        vstEvent->flags = kVstMidiEventIsRealtime;
    }
}

void VstPlugin::process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &outBuffer, const Midi::MidiBuffer& midiBuffer){

    Q_UNUSED(in)
    //qCDebug(vst) << "processing ...";
    if( isBypassed() || !effect || !loaded || !started){
        return;
    }

    if(!turnedOn){
        resume();
        turnedOn = true;
    }

    if(wantMidi){
        fillVstEventsList(midiBuffer);//translate midiBuffer messages in VstEvents
        effect->dispatcher(effect, effProcessEvents, 0, 0, (void*)&vstMidiEvents, 0);
    }

    internalOutputBuffer->setFrameLenght(outBuffer.getFrameLenght());
    internalInputBuffer->setFrameLenght(outBuffer.getFrameLenght());

    internalInputBuffer->set(in);

    //if(!isBypassed()){
        int inChannels = internalInputBuffer->getChannels();
        for (int c = 0; c < inChannels; ++c) {
            vstInputArray[c] = internalInputBuffer->getSamplesArray(c);
        }

        int outChannels = internalOutputBuffer->getChannels();
        for (int c = 0; c < outChannels; ++c) {
            vstOutputArray[c] = internalOutputBuffer->getSamplesArray(c);
        }

        VstInt32 sampleFrames = outBuffer.getFrameLenght();
        if(effect->flags & effFlagsCanReplacing){
            QMutexLocker locker(&editorMutex);
            effect->processReplacing(effect, vstInputArray, vstOutputArray, sampleFrames);
        }
    //}
    //else{//bypassed, just copy in to out   TODO: copy in directly to outBuffer, avoid copy to internalOut and after this copy to outBuffer
    //    internalOutputBuffer->set(*internalInputBuffer);
    //}

    outBuffer.add(*internalOutputBuffer);
    //outBuffer.set(*internalOutputBuffer);
}

void VstPlugin::setBypass(bool state){
    Plugin::setBypass(state);
    if(effect){
        effect->dispatcher(effect, effSetBypass, 0, state, NULL, 0);
    }
}

void VstPlugin::closeEditor(){
    qCDebug(vst) << "Closing " << getName() << " editor. Thread:" << QThread::currentThreadId();
    QMutexLocker locker(&editorMutex);
    if(effect && editorWindow){
        effect->dispatcher(effect, effEditClose, 0, 0, NULL, 0);
    }
    Audio::Plugin::closeEditor();
    qCDebug(vst) << "Editor closed";
}

void VstPlugin::openEditor(QPoint centerOfScreen){
    if(!effect ){
        return;
    }

    if(!(effect->flags & effFlagsHasEditor)){
        return;
    }

    QMutexLocker locker(&editorMutex);

    if(editorWindow && editorWindow->isVisible()){
        editorWindow->raise();
        editorWindow->activateWindow();
        return;
    }

    qCDebug(vst) << "opening " <<getName() << "editor thread: " << QThread::currentThreadId();

    if(editorWindow){
        editorWindow->deleteLater();
    }
    this->editorWindow = new QDialog(0, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    this->editorWindow->setWindowTitle(getName());
    QObject::connect( this->editorWindow, SIGNAL(finished(int)), this, SLOT(editorDialogFinished()));

    //obtém o tamanho da janela do plugin
    ERect* rect;
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
    if (!rect) {
      qCCritical(vst) << "VST plugin returned NULL edit rect";
      return;
    }
    int rectWidth = rect->right - rect->left;
    int rectHeight = rect->bottom - rect->top;

    this->editorWindow->setFixedSize(rectWidth, rectHeight);

    this->editorWindow->show();

    effect->dispatcher(effect, effEditOpen, 0, 0, (void*)(editorWindow->effectiveWinId()), 0);

    //Some plugins don't return the real size until after effEditOpen
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
	if (rect) {
        editorWindow->setFixedSize(rect->right - rect->left, rect->bottom - rect->top);

		rectWidth = rect->right - rect->left;
		rectHeight = rect->bottom - rect->top;

        editorWindow->move(centerOfScreen.x() - rectWidth / 2, centerOfScreen.y() - rectHeight / 2);
	}

    qCDebug(vst) << getName() << " editor opened";



    //resume();
}

void VstPlugin::updateGui(){
    if(isBypassed() || !effect || !loaded || !started){
        return;
    }
    if(!editorWindow || !editorWindow->isVisible()){
        return;
    }

    effect->dispatcher(effect, effEditIdle, 0, 0, 0, 0);
}
