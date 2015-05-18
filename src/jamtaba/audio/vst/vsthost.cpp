#include "vsthost.h"
#include "aeffectx.h"
#include "../midi/MidiDriver.h"
#include "portmidi.h"

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
    this->vstMidiEvents.reserved = 0;
    this->vstMidiEvents.numEvents = 0;
    //this->vstMidiEvents.events = new VstEvent*[MAX_EVENTS];
    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        this->vstMidiEvents.events[i] = (VstEvent*)(new VstMidiEvent);
    }
}

const VstEvents* VstHost::getVstMidiEvents() const{
    return (VstEvents*)&vstMidiEvents;
}

void VstHost::fillMidiEvents(Midi::MidiBuffer &midiIn){
    int midiMessages = std::min( midiIn.getMessagesCount(), MAX_MIDI_EVENTS);
    this->vstMidiEvents.numEvents = midiMessages;
    for (int m = 0; m < midiMessages; ++m) {
        Midi::MidiMessage message = midiIn.consumeMessage();
        VstMidiEvent* vstEvent = (VstMidiEvent*)vstMidiEvents.events[m];
        vstEvent->type = kVstMidiType;
        vstEvent->byteSize = sizeof(vstEvent);
        vstEvent->deltaFrames = 0;
        vstEvent->midiData[0] = Pm_MessageStatus(message.data);
        vstEvent->midiData[1] = Pm_MessageData1(message.data);
        vstEvent->midiData[2] = Pm_MessageData2(message.data);
        vstEvent->midiData[3] = 0;
        vstEvent->reserved1 = vstEvent->reserved2 = 0;
        vstEvent->flags = kVstMidiEventIsRealtime;
    }
}

VstHost::~VstHost()
{
    delete this->vstTimeInfo;
    for (int i = 0; i < MAX_MIDI_EVENTS; ++i) {
        delete this->vstMidiEvents.events[i];
    }
//    delete [] this->vstMidiEvents.events;
}

void VstHost::setBlockSize(int blockSize){
    this->blockSize = blockSize;
}

void VstHost::setSampleRate(int sampleRate){
    this->sampleRate = sampleRate;
}

VstIntPtr VSTCALLBACK VstHost::hostCallback(AEffect */*effect*/, VstInt32 opcode,
  VstInt32 /*index*/, VstInt32 /*value*/, void *ptr, float /*opt*/)
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

    }
    return 0L;
}
