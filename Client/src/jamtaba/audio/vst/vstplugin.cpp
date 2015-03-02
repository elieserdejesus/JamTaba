#include "vstplugin.h"
#include "aeffectx.h"
#include <windows.h>
#include <QDebug>
#include "../audio/core/AudioDriver.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QDesktopWidget>

using namespace Vst;

//+++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
VstPlugin::VstPlugin(QString name, QString file, AEffect *effect)
    :Audio::Plugin(name, file), effect(effect)
{

}

VstPlugin::~VstPlugin()
{
    delete effect;
}

void VstPlugin::process(Audio::SamplesBuffer &buffer){
    float** in = buffer.getSamplesArray();
    float** out = in;
    VstInt32 sampleFrames = buffer.getFrameLenght();
    effect->processReplacing(effect, in, out, sampleFrames);
           //(AEffect* effect, float** inputs, float** outputs, VstInt32 sampleFrames);
}

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

void VstPlugin::openEditor(Audio::PluginWindow *w, QPoint p){
    //abre o editor do plugin usando o handle do diálogo
    effect->dispatcher(effect, effEditOpen, 0, 0, (void*)(w->effectiveWinId()), 0);

    //obtém o tamanho da janela do plugin
    ERect* rect;
    effect->dispatcher(effect, effEditGetRect, 0, 0, (void*)&rect, 0);
    int rectWidth = rect->right - rect->left;
    int rectHeight = rect->bottom - rect->top;
    w->setFixedSize(rectWidth, rectHeight);

    QDesktopWidget widget;
    QRect mainScreenSize = widget.availableGeometry(widget.primaryScreen());

    int dialogX = p.x();
    if(mainScreenSize.width() - dialogX < rectWidth){
        if(rectWidth < mainScreenSize.width()){
            dialogX = (mainScreenSize.width()-rectWidth)/2;
        }
        else{
            dialogX = 0;
        }
    }

    int dialogY = p.y();
    if(mainScreenSize.height() - dialogY < rectHeight){
        if(rectHeight < mainScreenSize.height()){
            dialogY = (mainScreenSize.height()-rectHeight)/2;
        }
        else{
            dialogY = 0;
        }
    }

    w->move(dialogX, dialogY);
    delete rect;
}

// Plugin's entry point
typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

//// Plugin's dispatcher function
typedef VstIntPtr (*dispatcherFuncPtr)(AEffect *effect, VstInt32 opCode,
  VstInt32 index, VstInt32 value, void *ptr, float opt);

Vst::VstPlugin* Vst::load(QString pluginPath){
    AEffect *plugin = nullptr;

    HINSTANCE modulePtr = LoadLibrary(pluginPath.toStdWString().c_str());
    if(modulePtr == NULL) {
        qCritical() <<"Failed to load VST from "<< pluginPath << " error:" << GetLastError();
        return NULL;
    }

    vstPluginFuncPtr mainEntryPoint =
            (vstPluginFuncPtr)GetProcAddress(modulePtr, "VSTPluginMain");
    // Instantiate the plugin
    plugin = mainEntryPoint(hostCallback);
    if(plugin->magic != kEffectMagic){
        qFatal("plugin magic is wrong");
    }

    dispatcherFuncPtr dispatcher = (dispatcherFuncPtr)(plugin->dispatcher);

    //void startPlugin(AEffect *plugin) {
      dispatcher(plugin, effOpen, 0, 0, NULL, 0.0f);

      // Set some default properties
      float sampleRate = 44100.0f;
      dispatcher(plugin, effSetSampleRate, 0, 0, NULL, sampleRate);
      int blocksize = 2048;
      dispatcher(plugin, effSetBlockSize, 0, blocksize, NULL, 0.0f);

      //void resumePlugin(AEffect *plugin) {
        dispatcher(plugin, effMainsChanged, 0, 1, NULL, 0.0f);

        //virtual long EffEditOpen(void *ptr) { long l = EffDispatch(effEditOpen, 0, 0, ptr); /* if (l > 0) */ bEditOpen = true; return l; }

      //}

      //void suspendPlugin(AEffect *plugin) {
//        dispatcher(plugin, effMainsChanged, 0, 0, NULL, 0.0f);
//      }

      //resume();
    //}




    return new Vst::VstPlugin("Nome de teste", pluginPath, plugin);
}
