#include <QCoreApplication>
#include <QLibrary>
#include <QDebug>

#include "aeffectx.h"

long VSTCALLBACK hostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
    qDebug() << "opcode: " << opcode;

    switch(opcode) {
    case 6: //audioMasterWantMidi
        return true;

    case audioMasterIdle:
            return 0L;

    case audioMasterVersion : //1
        return 2;
        //	return 2400L; // Supports VST v2.4
        //	return 2300L; // Supports VST v2.3
        //	return 2200L; // Supports VST v2.2
        //	return 2100L; // Supports VST v2.1

    case audioMasterGetBlockSize:
        return 256;

    case audioMasterGetSampleRate:
        return 44100;

    case audioMasterGetTime : //7
        return 0;

    case audioMasterGetCurrentProcessLevel : //23
        return 2L;

    case audioMasterGetVendorString : //32
        strcpy((char *)ptr,"www.jamtaba.com");
        return 1L;

    case audioMasterGetProductString : //33
        strcpy((char *)ptr,"Jamtaba II");
        return 1L;

    case audioMasterGetVendorVersion : //34
        return 1L;


    case audioMasterUpdateDisplay://42

        return 1L;

    case audioMasterCanDo : //37
        const char* str = (const char*)ptr;

        if ((!strcmp(str, "sendVstEvents")) ||
                (!strcmp(str, "sendVstMidiEvent")) ||
                //(!strcmp(str, "receiveVstEvents")) ||
                //(!strcmp(str, "receiveVstMidiEvent")) ||
                (!strcmp(str, "sizeWindow")) ||
                (!strcmp(str, "sendVstMidiEventFlagIsRealtime")) ||
                //(!strcmp(str, "reportConnectionChanges")) ||
                //(!strcmp(str, "acceptIOChanges")) ||
                (!strcmp(str, "sendVstTimeInfo")) ||
                (!strcmp(str, "supplyIdle"))
                //(!strcmp(str, "shellCategory"))
                ){
            return 1L;
        }

        //ignore the rest
        qDebug() << "host can't do"<<str;
        return 0;
    }
    qDebug() << "not handled hostCallBack opcode:" << opcode;
    return 0L;
}

typedef AEffect *(*vstPluginFuncPtr)(audioMasterCallback host);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QLibrary lib("../../VstPlugin/release/JamtabaVST2.dll");
    if(!lib.load()){
        qCritical() << lib.errorString();
        return -1;
    }
    qDebug() << "lib loaded!";

    vstPluginFuncPtr entry = (vstPluginFuncPtr)lib.resolve("VSTPluginMain");
    if(!entry){
        qCritical() << lib.errorString();
        return -1;
    }
    qDebug() << "entry point found!";
    AEffect* effect = entry((audioMasterCallback)hostCallback);
    if(!effect){
        qCritical() << "can't create effect!";
        return -1;
    }
    qDebug() << "Effect instance created!";

    effect->dispatcher (effect, effOpen, 0, 0, 0, 0);
    effect->dispatcher (effect, effSetSampleRate, 0, 0, 0, 44100);
    effect->dispatcher (effect, effSetBlockSize, 0, 256, 0, 0);


    if(lib.isLoaded()){
        lib.unload();
    }
    return a.exec();
}
