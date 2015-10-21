
#include "vsthost.h"
#include "aeffectx.h"
#include "../midi/MidiDriver.h"
//#include "portmidi.h"
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <cmath>

using namespace Vst;

Q_LOGGING_CATEGORY(vstHost, "vst.host")

QScopedPointer<Host> Host::hostInstance;

Host* Host::getInstance(){
    if(!hostInstance){
        hostInstance.reset( new Host());
    }
    return hostInstance.data();
}

Host::Host()
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

void Host::clearVstTimeInfoFlags(){
    int tempSampleRate = vstTimeInfo.sampleRate;

    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = tempSampleRate;
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

void Host::setPlayingFlag(bool playing){
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


void Host::update(int intervalPosition){
    vstTimeInfo.samplePos = intervalPosition;

    int measure = (int)vstTimeInfo.ppqPos/vstTimeInfo.timeSigNumerator;
    vstTimeInfo.barStartPos = measure * vstTimeInfo.timeSigNumerator;

    vstTimeInfo.smpteOffset = measure + 1;

    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.timeSigDenominator = 4;

    vstTimeInfo.nanoSeconds = QDateTime::currentDateTime().currentMSecsSinceEpoch() * 1000000.0;

    //++++++++++++++
    //bar length in quarter notes
    float barLengthq =  (float)(4*vstTimeInfo.timeSigNumerator)/vstTimeInfo.timeSigDenominator;

    vstTimeInfo.cycleEndPos = 0; //barLengthq*loopLenght;
    vstTimeInfo.cycleStartPos = 0;

    double dPos = vstTimeInfo.samplePos / vstTimeInfo.sampleRate;
    vstTimeInfo.ppqPos = dPos * vstTimeInfo.tempo / 60.L;

    int currentBar = std::floor(vstTimeInfo.ppqPos/barLengthq);
    vstTimeInfo.barStartPos = barLengthq*currentBar;
    //+++++++

    vstTimeInfo.flags = 0;
    vstTimeInfo.flags |= kVstTransportChanged;//     = 1,		///< indicates that play, cycle or record state has changed
    vstTimeInfo.flags |= kVstTransportPlaying;//     = 1 << 1,	///< set if Host sequencer is currently playing
    //vstTimeInfo.flags |= kVstTransportCycleActive;// = 1 << 2,	///< set if Host sequencer is in cycle mode
    //vstTimeInfo.flags |= kVstAutomationReading;//    = 1 << 7,	///< set if automation read mode active (play parameter changes)
    vstTimeInfo.flags |= kVstNanosValid;//           = 1 << 8,	///< VstTimeInfo::nanoSeconds valid
    vstTimeInfo.flags |= kVstPpqPosValid;//          = 1 << 9,	///< VstTimeInfo::ppqPos valid
    vstTimeInfo.flags |= kVstTempoValid;//           = 1 << 10,	///< VstTimeInfo::tempo valid
    vstTimeInfo.flags |= kVstBarsValid;//            = 1 << 11,	///< VstTimeInfo::barStartPos valid
    //vstTimeInfo.flags |= kVstCyclePosValid;//        = 1 << 12,	///< VstTimeInfo::cycleStartPos and VstTimeInfo::cycleEndPos valid
    vstTimeInfo.flags |= kVstTimeSigValid;//         = 1 << 13,	///< VstTimeInfo::timeSigNumerator and VstTimeInfo::timeSigDenominator valid
    //vstTimeInfo.flags |= kVstSmpteValid;//           = 1 << 14,	///< VstTimeInfo::smpteOffset and VstTimeInfo::smpteFrameRate valid
    vstTimeInfo.flags |= kVstClockValid;
}

Host::~Host()
{
    //delete this->vstTimeInfo;
    //    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
    //        delete this->vstMidiEvents.events[i];
    //    }
    //    delete [] this->vstMidiEvents.events;
}

void Host::setBlockSize(int blockSize){
    this->blockSize = blockSize;
}

void Host::setSampleRate(int sampleRate){
    this->vstTimeInfo.sampleRate = sampleRate;
}

int Host::getSampleRate() const{
    return (int)this->vstTimeInfo.sampleRate;
}

void Host::setTempo(int bpm){
    this->vstTimeInfo.tempo = bpm;
    this->vstTimeInfo.flags |= kVstTempoValid;
}

bool Host::tempoIsValid() const{
    return this->vstTimeInfo.flags & kVstTempoValid;
}

long VSTCALLBACK Host::hostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt)
{
    Q_UNUSED(effect)
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(opt)

    qCDebug(vstHost) << "opcode: " << opcode;

    switch(opcode) {

    case audioMasterIdle:
            QApplication::processEvents();
            return 0L;

    case audioMasterVersion : //1
        return 2;
        //	return 2400L; // Supports VST v2.4
        //	return 2300L; // Supports VST v2.3
        //	return 2200L; // Supports VST v2.2
        //	return 2100L; // Supports VST v2.1

    case audioMasterGetBlockSize:
        return Host::getInstance()->blockSize;

    case audioMasterGetSampleRate:
        return Host::getInstance()->getSampleRate();

    case audioMasterWantMidi://6
        return true;

    case audioMasterGetTime : //7
        return (long)(&Host::getInstance()->vstTimeInfo);

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
        QApplication::processEvents();
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
        qCDebug(vstHost) << "host can't do"<<str;
        return 0;
    }
    qCDebug(vstHost) << "not handled hostCallBack opcode:" << opcode;
    return 0L;
}
