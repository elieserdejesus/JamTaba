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


int PortMidiDriver::getInputDeviceIndex() const{
    return deviceId;
}

void PortMidiDriver::setInputDeviceIndex(int index){
    int totalDevices = Pm_CountDevices();
    if(index >= 0 && index < totalDevices){
        stop();
        deviceId = pmInvalidDeviceId;
        int inputIndex = 0;
        for (int i = 0; i < totalDevices; ++i) {
            if(Pm_GetDeviceInfo(index)->input > 0 && inputIndex == index){//index is a input device?
                deviceId = index;
                break;
            }
            inputIndex++;
        }
        if(deviceId != pmInvalidDeviceId){
            start();
        }
        else{
            qCritical() << "não foi possível reinicializar o device MIDI, indice errado";
        }
    }
}

void PortMidiDriver::start(){
    stop();
    if(deviceId == pmInvalidDeviceId){
        deviceId = Pm_GetDefaultInputDeviceID();
    }
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
    int totalDevices = Pm_CountDevices();
    int inputDevices = 0;
    for (int i = 0; i < totalDevices; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if(info->input > 0){
            inputDevices++;
        }
    }
    return inputDevices;
}

const char* PortMidiDriver::getInputDeviceName(int index) const{
    int totalDevices = Pm_CountDevices();
    int inputIndex = -1;
    for (int i = 0; i < totalDevices; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if(info->input > 0 && inputIndex == index){
            return info->name;
        }
        inputIndex++;
    }
    return "wrong index";
}
