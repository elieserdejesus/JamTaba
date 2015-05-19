#include "portmididriver.h"

#include <QDebug>

using namespace Midi;

PortMidiDriver::PortMidiDriver()

{
    deviceId = pmInvalidDeviceId;
    stream = nullptr;
    Pm_Initialize();

//    int devices = Pm_CountDevices();
//    qDebug() << "MIDI DEVICEs";
//    for (int d = 0; d < devices; ++d) {
//        const PmDeviceInfo* info = Pm_GetDeviceInfo(d);
//        qDebug() << info->name << " (" << info->interf << ") " << " in: " << info->input << " out: " << info->output << endl;
//    }
//    qDebug() << "---------------" << endl;
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
    //A port Midi não diferencia entrada de saida, então os índices precisam ser convertidos. Por exemplo, usar a entrada
    //de índice 0 pode significar usar o device de índice 4, porque os devices anteriores são todos saída
    int totalDevices = Pm_CountDevices();
    if(index >= 0 && index < totalDevices){
        bool wasStarted = stream != nullptr;
        if(wasStarted){
            stop();
        }
        deviceId = pmInvalidDeviceId;
        int inputIndex = 0;
        for (int i = 0; i < totalDevices; ++i) {
            if(Pm_GetDeviceInfo(i)->input > 0){//index is a input device?
                if( inputIndex == index){
                    deviceId = i;
                    break;
                }
                inputIndex++;
            }
        }
        if(deviceId != pmInvalidDeviceId){
            if(wasStarted){
                start();
            }
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
        if(Pm_HasHostError(stream)){
            char msg[64];
            Pm_GetHostErrorText(msg, 64);
            qWarning() << endl << msg << endl << endl;
        }
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
        stream = nullptr;
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
