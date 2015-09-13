
#include "vsthost.h"
#include "aeffectx.h"
#include "../midi/MidiDriver.h"
#include "portmidi.h"
#include <QDebug>
#include <QDateTime>

using namespace Vst;

Q_LOGGING_CATEGORY(vstHost, "vst.host")

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
//    qCritical() << "VstHost Construtor";
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

    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = this->getSampleRate();
    vstTimeInfo.nanoSeconds = 0.0;
    vstTimeInfo.ppqPos = 0.0;
    vstTimeInfo.tempo = 120.0;
    vstTimeInfo.barStartPos = 0.0;
    vstTimeInfo.cycleStartPos = 0.0;
    vstTimeInfo.cycleEndPos = 0.0;
    vstTimeInfo.timeSigNumerator = 4;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.smpteOffset = 0;
    vstTimeInfo.smpteFrameRate = 1;
    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.flags = 0;
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

    vstTimeInfo.samplePos = intervalPosition;
    double quarterTime = 60000.0/vstTimeInfo.tempo;
    double samplesPerQuarter = vstTimeInfo.sampleRate * quarterTime / 1000;

    vstTimeInfo.ppqPos =  intervalPosition/samplesPerQuarter;
    int measure = (int)vstTimeInfo.ppqPos/vstTimeInfo.timeSigNumerator;
    vstTimeInfo.barStartPos = measure * vstTimeInfo.timeSigNumerator;
    //the ppq value returned by vsttimeinfo is a float which will vary from 0 to 3.999999999(etc) over the 4 beats of a typical 16 semiquaver bar of 4/4.

    vstTimeInfo.smpteOffset = measure + 1;

    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.timeSigDenominator = 4;

    vstTimeInfo.nanoSeconds = QDateTime::currentDateTime().currentMSecsSinceEpoch()/1000.0;

    vstTimeInfo.flags |= kVstPpqPosValid;
    vstTimeInfo.flags |= kVstTempoValid;
    vstTimeInfo.flags |= kVstBarsValid;
    vstTimeInfo.flags |= kVstTransportPlaying;
    vstTimeInfo.flags |= kVstNanosValid;
    vstTimeInfo.flags |= kVstTimeSigValid;


    //kVstCyclePosValid        = 1 << 12,	///< VstTimeInfo::cycleStartPos and VstTimeInfo::cycleEndPos valid
    //kVstTimeSigValid         = 1 << 13,	///< VstTimeInfo::timeSigNumerator and VstTimeInfo::timeSigDenominator valid
    //kVstSmpteValid           = 1 << 14,	///< VstTimeInfo::smpteOffset and VstTimeInfo::smpteFrameRate valid
    //kVstClockValid           = 1 << 15	///< VstTimeInfo::samplesToNextClock valid
    //vstTimeInfo.flags |= kVstTransportChanged;
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
    this->vstTimeInfo.sampleRate = sampleRate;
}

int VstHost::getSampleRate() const{
    return (int)this->vstTimeInfo.sampleRate;
}

void VstHost::setTempo(int bpm){
    this->vstTimeInfo.tempo = bpm;
    this->vstTimeInfo.flags |= kVstTempoValid;
}

bool VstHost::tempoIsValid() const{
    return this->vstTimeInfo.flags & kVstTempoValid;
}

long VSTCALLBACK VstHost::hostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
    Q_UNUSED(effect)
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(opt)

    qCDebug(vstHost) << "hostCallBack opcode:" << opcode;

    const char* str;
    switch(opcode) {
        case audioMasterVersion : //1
            return 2400L;

        case audioMasterGetBlockSize:
            return VstHost::getInstance()->blockSize;

        case audioMasterGetSampleRate:
            return VstHost::getInstance()->getSampleRate();

//        case audioMasterWantMidi:
//            return 1L;

        case audioMasterGetTime : //7
    {
        if(VstHost::getInstance()->tempoIsValid()){
            //retornar uma copia como sugerido no kvr. Ver o wahjam e o vst host

            //VstTimeInfo hostTimeInfo = ;
           return (long)(&(VstHost::getInstance()->vstTimeInfo));
        }
        qCDebug(vstHost) << "Returning 0 for VstTime";
        return 0L;//avoid crash some plugins
    }

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
                //(!strcmp(str, "reportConnectionChanges")) ||
                //(!strcmp(str, "acceptIOChanges")) ||
                (!strcmp(str, "sendVstTimeInfo"))
                //(!strcmp(str, "openFileSelector")) ||
                //(!strcmp(str, "closeFileSelector")) ||
                //(!strcmp(str, "supplyIdle")) ||
                //(!strcmp(str, "receiveVstTimeInfo")) ||
                //(!strcmp(str, "shellCategory")) ||
                ){
                qCDebug(vstHost) << "Host CAN DO " << str;
                return 1L;
            }


            //ignore the rest
            qCDebug(vstHost) << "host can't do"<<str;

    }
    return 0L;
}
