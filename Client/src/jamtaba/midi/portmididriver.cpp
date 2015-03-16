#include "portmididriver.h"

#include <QDebug>

using namespace Midi;

PortMidiDriver::PortMidiDriver()

{
    deviceId = pmInvalidDeviceId;
    stream = nullptr;
    Pm_Initialize();
}

PortMidiDriver::~PortMidiDriver()
{
    stop();
    release();
}


void PortMidiDriver::start(){
    stop();
    deviceId = Pm_GetDefaultInputDeviceID();
    if(deviceId != pmInvalidDeviceId){
        const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(deviceId);
        if(deviceInfo != NULL){
            qDebug() << "Iniciando MIDI em " << Pm_GetDeviceInfo(deviceId)->name;
            Pm_OpenInput(&stream, deviceId, nullptr, 256, nullptr, nullptr );
        }
        else{
            qCritical() << "Não foi possível inicializar algum dispositivo MIDI";
        }
    }


}

MidiBuffer PortMidiDriver::getBuffer(){

    MidiBuffer buffer(256);
    if(stream){
        while(Pm_Poll(stream) == pmGotData){//has data to read
            PmEvent event;
            int eventsReaded = Pm_Read(stream, &event, 1);
            if(eventsReaded > 0){
                buffer.addMessage(MidiMessage(event.message, event.timestamp));
            }
        }
    }
    return buffer;
}



void PortMidiDriver::stop(){
    if(stream ){
        Pm_Abort(stream);
    }
}

void PortMidiDriver::release(){
    stop();
    Pm_Terminate();
}

int PortMidiDriver::getMaxInputDevices() const{
    return Pm_CountDevices();
}

const char* PortMidiDriver::getInputDeviceName(int index) const{
    if(index >= 0 && index < Pm_CountDevices()){
        const PmDeviceInfo* info = Pm_GetDeviceInfo(index);
        return info->name;
    }
    return "wrong index";
}
