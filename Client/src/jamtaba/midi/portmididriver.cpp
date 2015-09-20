#include "portmididriver.h"


#include <QDebug>

Q_LOGGING_CATEGORY(midi, "midi")

using namespace Midi;

PortMidiDriver::PortMidiDriver(QList<bool> deviceStatuses)
{
    qCDebug(midi) << "Initializing portmidi...";
    Pm_Initialize();
    int maxInputDevices = getMaxInputDevices();
    if(deviceStatuses.size() < maxInputDevices){
        int itemsToAdd = maxInputDevices - deviceStatuses.size();
        for (int i = 0; i < itemsToAdd; ++i) {
            deviceStatuses.append(true);
        }
    }
    setInputDevicesStatus(deviceStatuses);
    qCDebug(midi) << "portmidi... initialized";
}

PortMidiDriver::~PortMidiDriver()
{
    stop();
    release();
}


void PortMidiDriver::setInputDevicesStatus(QList<bool> statuses){
    MidiDriver::setInputDevicesStatus(statuses);
    foreach (PmStream* stream, streams) {
        if(stream){
            Pm_Abort( stream );
            Pm_Close( stream );
        }
    }
    streams.clear();
}

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

    for(int globalDeviceIndex=0; globalDeviceIndex < inputDevicesEnabledStatuses.size(); globalDeviceIndex++) {
        if(inputDevicesEnabledStatuses.at(globalDeviceIndex)){//device is globally enabled?
            PmDeviceID deviceId = getDeviceIDFromGlobalIndex(globalDeviceIndex);
            const PmDeviceInfo* deviceInfo = Pm_GetDeviceInfo(deviceId);
            if(deviceInfo){
                //qWarning() << "Iniciando MIDI em " << Pm_GetDeviceInfo(deviceId)->name;
                PmStream* stream;
                Pm_OpenInput(&stream, deviceId, nullptr, 256, nullptr, nullptr );

                streams.append(stream);
            }
            else{
                streams.append(nullptr);//just to keep correct size in streams list
            }
        }
        else{
            streams.append(nullptr);//just to keep correct size in streams list
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
        if(deviceIsGloballyEnabled(globalDeviceID) && stream){
            if(Pm_HasHostError(stream)){
                char msg[64];
                Pm_GetHostErrorText(msg, 64);
                qWarning() << endl << msg << endl << endl;
            }
            while(Pm_Poll(stream) == pmGotData){//has data to read
                PmEvent events[BUFFER_SIZE];
                int eventsReaded = Pm_Read(stream, events, BUFFER_SIZE);
                for(int e = 0; e < eventsReaded; ++e){
                    //int status = Pm_MessageStatus(events[e].message);
                    //int channel = status & 0x0000000F;
                    //qWarning() << channel;
                    buffer.addMessage(MidiMessage(events[e].message, events[e].timestamp, globalDeviceID));
                }
            }

        }
        globalDeviceID++;//process next device midi messages
    }
    return buffer;
}

void PortMidiDriver::stop(){
    foreach (PmStream* stream, streams) {
        if(stream){
            Pm_Close(stream);
            //Pm_Abort(stream);
        }
    }
    streams.clear();
}

void PortMidiDriver::release(){
    stop();
    foreach (PmStream* stream, streams) {
        if(stream){
            Pm_Close(stream);
        }
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
    int inputIndex = 0; //-1 change to 0 to fix a bug in mac
    for (int i = 0; i < totalDevices; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        if(info->input > 0){
            if(inputIndex == index){
                return info->name;
            }
            inputIndex++;
        }

    }
    return "wrong index";
}
