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

AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

//++++++++++++++++++++++++++++++++

class Jamtaba;

class JamtabaWindow : public QWinWidget{
    Q_OBJECT
public:
    JamtabaWindow(Jamtaba *jamtaba, HWND parent = NULL);
private slots:
    void on_sliderChanged(int value);
private:
    HWND h_parent;
    Jamtaba *jamtaba;

    QLabel *valueLabel;
    QSlider *slider;
};

class JamtabaEditor : public AEffEditor{
    QWinWidget* widget;
    Jamtaba* jamtaba;
    ERect rectangle;

public:
    JamtabaEditor(Jamtaba* jamtaba);
    ~JamtabaEditor();
    bool getRect (ERect** rect);
    void clientResize(HWND h_parent, int width, int height);
    bool open(void* ptr);
    void close();
};


//-------------------------------------------------------------------------------------------------------
class Jamtaba :  public AudioEffectX
{
//    Q_OBJECT
public:

        Jamtaba (audioMasterCallback audioMaster);
        ~Jamtaba ();

        VstInt32 canDo(char* text);
        //VstInt32 processEvents(VstEvents* events);

        void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames);
        void processDoubleReplacing (double** inputs, double** outputs, VstInt32 sampleFrames){}

        VstInt32 fxIdle();
        bool needIdle();

        inline void setGain(float newGain){this->gain = newGain;}

        //void setProgramName (char* name){}
        //void getProgramName (char* name){}
        //void setProgram(VstInt32 program){}
        //VstInt32 getProgram(){ return 0;}

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
        VstPlugCategory getPlugCategory() {return kPlugCategEffect;}

        void updateParameter(int index, float value);



protected:
        //QApplication *myApp;
        //MainHostVst *myHost;
        //MainWindowVst *myWindow;

        char programName[kVstMaxProgNameLen + 1];
        VstInt32 bufferSize;

        //Gui *qEditor;

        VstEvents *listEvnts;

        bool hostSendVstEvents;
        bool hostSendVstMidiEvent;
        bool hostReportConnectionChanges;
        bool hostAcceptIOChanges;
        bool hostSendVstTimeInfo;
        bool hostReceiveVstEvents;
        bool hostReceiveVstMidiEvents;
        bool hostReceiveVstTimeInfo;
        bool opened;
        //VstInt32 currentHostProg;

        //char *chunkData;
        //void deleteChunkData();
private:
        //QVstGui* editor;
        float gain;
};


#endif // VST_H
