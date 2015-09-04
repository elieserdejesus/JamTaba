#include "vsthost.h"
#include "aeffectx.h"
#include "../midi/MidiDriver.h"
#include "portmidi.h"
#include <QDebug>

using namespace Vst;

VstHost* VstHost::hostInstance = nullptr;


VstHost* VstHost::getInstance(){
    if(!hostInstance){
        hostInstance = new VstHost();
    }
    return hostInstance;
}

VstHost::VstHost()
    : blockSize(0)
{
//    this->vstMidiEvents.reserved = 0;
//    this->vstMidiEvents.numEvents = 0;
//    //this->vstMidiEvents.events = new VstEvent*[MAX_EVENTS];
//    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
//        this->vstMidiEvents.events[i] = (VstEvent*)(new VstMidiEvent);
//    }
    clearVstTimeInfoFlags();
}

void VstHost::clearVstTimeInfoFlags(){
    int tempSampleRate = vstTimeInfo.sampleRate;

    memset(&vstTimeInfo, 0, sizeof(vstTimeInfo));
    vstTimeInfo.timeSigNumerator = 4;
    vstTimeInfo.timeSigDenominator = 4;

    vstTimeInfo.sampleRate = tempSampleRate;
    vstTimeInfo.flags |= kVstTimeSigValid;
    vstTimeInfo.flags |= kVstPpqPosValid;
    vstTimeInfo.flags |= kVstTempoValid;
    vstTimeInfo.flags |= kVstBarsValid;
}

void VstHost::setPlayingFlag(bool playing){
    if(playing){
        vstTimeInfo.flags |= kVstTransportPlaying;
    }
    else{
        clearVstTimeInfoFlags();
    }
}

//void VstHost::setTransportChangedFlag(bool transportChanged){
//    if(transportChanged){
//        vstTimeInfo.flags |= kVstTransportChanged;
//    }
//    else{
//        vstTimeInfo.flags = vstTimeInfo.flags & (~kVstTransportChanged);
//    }
//}


void VstHost::update(int intervalPosition){
    //qWarning() << "updating";
    vstTimeInfo.samplePos = intervalPosition;
    double quarterTime = 60000.0/vstTimeInfo.tempo;
    double samplesPerQuarter = vstTimeInfo.sampleRate * quarterTime / 1000;

    vstTimeInfo.ppqPos =  intervalPosition/samplesPerQuarter;
    vstTimeInfo.barStartPos = ((int)vstTimeInfo.ppqPos/vstTimeInfo.timeSigNumerator) * vstTimeInfo.timeSigNumerator;
    //the ppq value returned by vsttimeinfo is a float which will vary from 0 to 3.999999999(etc) over the 4 beats of a typical 16 semiquaver bar of 4/4.

    vstTimeInfo.flags |= kVstPpqPosValid;
    vstTimeInfo.flags |= kVstBarsValid;
    vstTimeInfo.flags |= kVstTempoValid;
    vstTimeInfo.flags |= kVstTransportPlaying;
}

VstHost::~VstHost()
{
    //delete this->vstTimeInfo;
//    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
//        delete this->vstMidiEvents.events[i];
//    }
//    delete [] this->vstMidiEvents.events;
}

void VstHost::setBlockSize(int blockSize){
    this->blockSize = blockSize;
}

void VstHost::setSampleRate(int sampleRate){
    //this->sampleRate = sampleRate;
    this->vstTimeInfo.sampleRate = sampleRate;
}

int VstHost::getSampleRate() const{
    return (int)this->vstTimeInfo.sampleRate;
}

void VstHost::setTempo(int bpm){
    this->vstTimeInfo.tempo = bpm;
    this->vstTimeInfo.flags |= kVstTempoValid;
}

//static long VSTCALLBACK AudioMasterCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);

VstInt32 VSTCALLBACK VstHost::hostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstInt32 value, void *ptr, float opt)
{
    Q_UNUSED(effect)
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(opt)

    const char* str;
    switch(opcode) {
        case audioMasterVersion : //1
            return 2400L;

        case audioMasterGetBlockSize:
            return VstHost::getInstance()->blockSize;

        case audioMasterGetSampleRate:
            return VstHost::getInstance()->getSampleRate();


        case audioMasterGetTime : //7
            //qWarning() << "retornando timeInfo";
            return (VstIntPtr)(&VstHost::getInstance()->vstTimeInfo);

//        case audioMasterSetTime  : //9
//            pHost->SetTimeInfo((VstTimeInfo*)ptr);
//            return 1L;

//        case audioMasterTimeAt : //10
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

    }
    return 0L;
}
