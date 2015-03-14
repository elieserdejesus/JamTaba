#include "vsthost.h"
#include "aeffectx.h"

using namespace Vst;

VstHost* VstHost::hostInstance = nullptr;

VstHost* VstHost::getInstance(){
    if(!hostInstance){
        hostInstance = new VstHost();
    }
    return hostInstance;
}

VstHost::VstHost()
    :vstTimeInfo(new VstTimeInfo()),
      sampleRate(0), blockSize(0)
{

}

VstHost::~VstHost()
{
    delete this->vstTimeInfo;
}

void VstHost::setBlockSize(int blockSize){
    this->blockSize = blockSize;
}

void VstHost::setSampleRate(int sampleRate){
    this->sampleRate = sampleRate;
}

VstIntPtr VSTCALLBACK VstHost::hostCallback(AEffect *effect, VstInt32 opcode,
  VstInt32 index, VstInt32 value, void *ptr, float opt)
{
    //long retValue = 0L;
    const char* str;

    switch(opcode) {
        case audioMasterVersion : //1
            return 2400L;

        case audioMasterGetTime : //7
            return (long)(VstHost::getInstance()->vstTimeInfo);

//        case audioMasterSetTime  : //9
//            pHost->SetTimeInfo((VstTimeInfo*)ptr);
//            return 1L;

//        case audioMasterTempoAt : //10
//            return 1000L*pHost->vstTimeInfo.tempo;

        case audioMasterGetCurrentProcessLevel : //23
            return 2L;

        case audioMasterGetVendorString : //32
            strcpy((char *)ptr,"www.jamtaba.com");
            return 1L;

        case audioMasterGetProductString : //33
            strcpy((char *)ptr,"Jamtaba II");
            return 1L;

        case audioMasterGetVendorVersion : //34
            return 0001L;



        case audioMasterCanDo : //37
            str = (const char*)ptr;

            if ((!strcmp(str, "sendVstEvents")) ||
                (!strcmp(str, "sendVstMidiEvent")) ||
                (!strcmp(str, "receiveVstEvents")) ||
                (!strcmp(str, "receiveVstMidiEvent")) ||
                (!strcmp(str, "sizeWindow")) ||
                (!strcmp(str, "sendVstMidiEventFlagIsRealtime")) ||
                (!strcmp(str, "reportConnectionChanges")) ||
                (!strcmp(str, "acceptIOChanges")) ||
                (!strcmp(str, "sendVstTimeInfo")) ||
                //(!strcmp(str, "openFileSelector")) ||
                //(!strcmp(str, "closeFileSelector")) ||
                (!strcmp(str, "supplyIdle")) ||
                //(!strcmp(str, "receiveVstTimeInfo")) ||
                (!strcmp(str, "shellCategory")) ||
                0)
                return 1L;

            //ignore the rest
            qDebug() << "host can't do"<<str;
            return 0L;
    }

}
