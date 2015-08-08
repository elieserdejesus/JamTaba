#include "portmididriver.h"

#include <QDebug>

using namespace Midi;

PortMidiDriver::PortMidiDriver()
{
    Pm_Initialize();

    //enable all input devices
    int maxDevices = getMaxInputDevices();
    for (int globalIndex = 0; globalIndex < maxDevices; ++globalIndex) {
        this->setDeviceGlobalEnabledStatus(globalIndex, true);
    }
}

PortMidiDriver::~PortMidiDriver()
{
    stop();
    release();
}


//int PortMidiDriver::getInputDeviceIndex() const{
//    return deviceId;
//}

int PortMidiDriver::getDeviceIDFromGlobalIndex(int globalIndex){
    //A port Midi não diferencia entrada de saida, então os índices precisam ser convertidos. Por exemplo, usar a entrada
    //de índice 0 pode significar usar o device de índice 4, porque os devices anteriores são todos saída
    int totalDevices = Pm_CountDevices();
    int inputDeviceID = -1;
    for (int i = 0; i < totalDevices; ++i) {
        if(Pm_GetDeviceInfo(i)->input > 0){//index is a input device?
            inputDeviceID++;
            if( inputDeviceID == globalIndex){
                return i;
            }
        }
    }
    return -1;
}

void PortMidiDriver::start(){
    if(!hasInputDevices()){
        return;
    }
    stop();

    foreach (int globalDeviceIndex, inputDevicesEnabledStatuses.keys()) {
        if(inputDevicesEnabledStatuses[globalDeviceIndex]){//device is globally enabled?
            PmDeviceID deviceId = getDeviceIDFromGlobalIndex(globalDeviceIndex);
            const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(deviceId);
            if(deviceInfo){
                qWarning() << "Iniciando MIDI em " << Pm_GetDeviceInfo(deviceId)->name;
                PmStream* stream;
                Pm_OpenInput(&stream, deviceId, nullptr, 256, nullptr, nullptr );

                streams.append(stream);
            }
        }

    }

}

bool PortMidiDriver::hasInputDevices() const{
    return getMaxInputDevices() > 0;
}

MidiBuffer PortMidiDriver::getBuffer(){
    static const int BUFFER_SIZE = 256;
    MidiBuffer buffer(BUFFER_SIZE);
    int globalDeviceID = 0;
    foreach (PmStream* stream, streams) {
        if(deviceIsGloballyEnabled(globalDeviceID)){
            if(Pm_HasHostError(stream)){
                char msg[64];
                Pm_GetHostErrorText(msg, 64);
                qWarning() << endl << msg << endl << endl;
            }
            while(Pm_Poll(stream) == pmGotData){//has data to read
                PmEvent events[BUFFER_SIZE];
                int eventsReaded = Pm_Read(stream, events, BUFFER_SIZE);
                for(int e = 0; e < eventsReaded; ++e){
                    buffer.addMessage(MidiMessage(events[e].message, events[e].timestamp, globalDeviceID));
                }
            }
            globalDeviceID++;//process next device midi messages
        }
    }
    return buffer;
}



void PortMidiDriver::stop(){
    foreach (PmStream* stream, streams) {
        Pm_Abort(stream);
    }
    //streams.clear();
}

void PortMidiDriver::release(){
    stop();
    foreach (PmStream* stream, streams) {
        Pm_Close(stream);
    }
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
