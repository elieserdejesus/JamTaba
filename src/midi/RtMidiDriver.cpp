#include "RtMidiDriver.h"
#include "RtMidi.h"

Q_LOGGING_CATEGORY(midi, "midi")

using namespace Midi;

RtMidiDriver::RtMidiDriver(QList<bool> deviceStatuses)
    : rtMidi(nullptr){

    try{
        this->rtMidi = new RtMidiIn();
    }
    catch(RtMidiError &error) {
        qCCritical(midi) << QString( error.getMessage().c_str());
    }


    qCDebug(midi) << "Initializing portmidi...";
    int maxInputDevices = getMaxInputDevices();
    if(deviceStatuses.size() < maxInputDevices){
        int itemsToAdd = maxInputDevices - deviceStatuses.size();
        for (int i = 0; i < itemsToAdd; ++i) {
            deviceStatuses.append(true);
        }
    }
    setInputDevicesStatus(deviceStatuses);
    qCDebug(midi) << "rtmidi... initialized";

}

void RtMidiDriver::setInputDevicesStatus(QList<bool> statuses){
    MidiDriver::setInputDevicesStatus(statuses);

    rtMidi->openPort(0);

    //    foreach (PmStream* stream, streams) {
//        if(stream){
//            Pm_Abort( stream );
//            Pm_Close( stream );
//        }
//    }
    //streams.clear();
}

void RtMidiDriver::start(){

}

void RtMidiDriver::stop(){

}

void RtMidiDriver::release(){

}

const char* RtMidiDriver::getInputDeviceName(int index) const{
    if(rtMidi){
         if(index >= 0 && index < rtMidi->getPortCount()){
             //qWarning() << QString(rtMidi->getPortName(index).c_str());
             return  rtMidi->getPortName((uint)index).c_str();
         }
    }
    return "error";
}

MidiBuffer RtMidiDriver::getBuffer(){
    if(rtMidi){
        std::vector<unsigned char> messages;
        rtMidi->getMessage(&messages);

    }
    return MidiBuffer(0);
}

bool RtMidiDriver::hasInputDevices() const{
    return getMaxInputDevices() > 0;
}

int RtMidiDriver::getMaxInputDevices() const{
    if(rtMidi){
        return this->rtMidi->getPortCount();
    }
    return 0;
}

RtMidiDriver::~RtMidiDriver(){
    if(this->rtMidi){
        delete rtMidi;
    }
}
