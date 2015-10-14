#ifndef VST_H
#define VST_H

#define uniqueIDEffect CCONST('V','b','P','l')
#define uniqueIDInstrument CCONST('V','b','I','s')
#define VST_EVENT_BUFFER_SIZE 1000
#define DEFAULT_INPUTS 1
#define DEFAULT_OUTPUTS 1

//#include <QObject>
#include "audioeffectx.h"
#include "aeffeditor.h"
#include <QWinWidget>
#include <QLabel>
#include <QSlider>
#include "../MainController.h"
#include "MainWindow.h"

AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

//++++++++++++++++++++++++++++++++

class JamtabaPlugin;

class JamtabaVstEditor : public AEffEditor{
    QWinWidget* widget;
    JamtabaPlugin* jamtaba;
    ERect rectangle;
    MainWindow* mainWindow;

public:
    JamtabaVstEditor(JamtabaPlugin* jamtaba);
    ~JamtabaVstEditor();
    bool getRect (ERect** rect);
    void clientResize(HWND h_parent, int width, int height);
    bool open(void* ptr);
    void close();
    void detachMainController();

};


//-------------------------------------------------------------------------------------------------------
class MainControllerVST;
class JamtabaPlugin :  public AudioEffectX{
public:
        JamtabaPlugin (audioMasterCallback audioMaster);
        ~JamtabaPlugin ();

        inline bool isRunning() const{return running;}

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
        void setBlockSize (VstInt32 blockSize);
        void suspend();
        void resume();
        inline VstPlugCategory getPlugCategory() {return kPlugCategEffect;}

        void updateParameter(int index, float value);

        inline Controller::MainController* getController() {return controller;}

protected:
        char programName[kVstMaxProgNameLen + 1];
        VstInt32 bufferSize;

        //Gui *qEditor;
        VstEvents *listEvnts;
private:
        Controller::MainController* controller;
        bool running;

};


#endif // VST_H
