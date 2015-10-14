#ifndef VST_PLUGIN_H
#define VST_PLUGIN_H

#define uniqueIDEffect CCONST('V','b','P','l')
#define uniqueIDInstrument CCONST('V','b','I','s')
#define VST_EVENT_BUFFER_SIZE 1000
#define DEFAULT_INPUTS 1
#define DEFAULT_OUTPUTS 1

#include "audioeffectx.h"
#include <QLabel>
#include <QSlider>
#include "../MainController.h"
#include "MainWindow.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(pluginVst)

AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

//++++++++++++++++++++++++++++++++

class JamtabaPlugin;



//-------------------------------------------------------------------------------------------------------
class MainControllerVST;
class JamtabaPlugin :  public AudioEffectX{
public:
        JamtabaPlugin (audioMasterCallback audioMaster);
        ~JamtabaPlugin ();

        inline bool isRunning() const{return running;}

        bool isRunningAsVstPlugin() const;

        void initialize();//called first time editor is opened

        VstInt32 canDo(char* text);
        //VstInt32 processEvents(VstEvents* events);

        void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
        //void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames){}

        VstInt32 fxIdle();
        bool needIdle();

        bool getEffectName (char* name);
        bool getVendorString (char* text);
        bool getProductString (char* text);
        VstInt32 getVendorVersion ();

        VstInt32 getNumMidiInputChannels();
        VstInt32 getNumMidiOutputChannels();

        void open();
        void close();
        void setSampleRate(float sampleRate);
        //void setBlockSize (VstInt32 blockSize);
        void suspend();
        void resume();
        inline VstPlugCategory getPlugCategory() {return kPlugCategEffect;}

        void updateParameter(int index, float value);

        inline Controller::MainController* getController() {return controller;}

protected:
        char programName[kVstMaxProgNameLen + 1];
        int sampleRate;

        //Gui *qEditor;
        VstEvents *listEvnts;
private:
        Controller::MainController* controller;
        bool running;
        Audio::SamplesBuffer inputBuffer;
        Audio::SamplesBuffer outputBuffer;

};


#endif // VST_H
