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


    qCDebug(midi) << "Initializing rtmidi...";
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

    if(!statuses.isEmpty()){
        rtMidi->openPort(0);
    }

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
    if(rtMidi){
        rtMidi->closePort();
    }
}

void RtMidiDriver::release(){
    if(rtMidi){
        rtMidi->closePort();
        delete rtMidi;
    }
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
    MidiBuffer buffer(32);//max 32 midi messages
    if(rtMidi){
        while(true){
            std::vector<unsigned char> message;
            double stamp = rtMidi->getMessage(&message);
            if(!message.empty()){
                int msgData = 0;
                msgData |= message.at(0);
                msgData |= message.at(1) << 8;
                msgData |= message.at(2) << 16;
                //            qDebug(midi) << "original msgData:";
                //            qDebug(midi) << "Status:" << message.at(0);
                //            qDebug(midi) << "data 1:" << message.at(1);
                //            qDebug(midi) << "data 2:" << message.at(2) << endl;

                int sourceIndexDevice = 0;//TODO change this

                buffer.addMessage(MidiMessage(msgData, qint32(stamp), sourceIndexDevice));

                //            qDebug(midi) << "msgData:" << msgData;
                //            qDebug(midi) << "Status:" << ((msgData) & 0xFF);
                //            qDebug(midi) << "data 1:" << (((msgData) >> 8) & 0xFF);
                //            qDebug(midi) << "data 2:" << (((msgData) >> 16) & 0xFF);
            }
            else{
                break;
            }
        }
    }
    return buffer;
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
