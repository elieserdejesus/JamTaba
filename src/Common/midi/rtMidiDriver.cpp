#include "RtMidiDriver.h"
#include "RtMidi.h"

using namespace Midi;

#include "log/Logging.h"

RtMidiDriver::RtMidiDriver(const QList<bool> &deviceStatuses){
    qCInfo(jtMidi) << "Initializing rtmidi...";
    QList<bool> statuses(deviceStatuses);
    int maxInputDevices = getMaxInputDevices();
    qCDebug(jtMidi) << "MIDI DEVICES FOUND idx:" << maxInputDevices;
    if(statuses.size() < maxInputDevices){
        int itemsToAdd = maxInputDevices - statuses.size();
        for (int i = 0; i < itemsToAdd; ++i) {
            statuses.append(true);
        }
    }
    setInputDevicesStatus(statuses);
    qCInfo(jtMidi) << "rtmidi initialized!";

}

void RtMidiDriver::setInputDevicesStatus(const QList<bool> &statuses){
    release();

    int midiDevicesCount = getMaxInputDevices();
    QList<bool> validStatuses;
    for (int i = 0; i < midiDevicesCount && i < statuses.size(); ++i) {
        validStatuses.append(statuses.at(i));
    }

    MidiDriver::setInputDevicesStatus(validStatuses);

    for (int s = 0; s < validStatuses.size(); ++s) {
        midiStreams.append(new RtMidiIn());
    }
}

void RtMidiDriver::start(const QList<bool> &deviceStatuses){
    setInputDevicesStatus(deviceStatuses);

    if(!hasInputDevices()){
        return;
    }
    stop();

    for(int deviceIndex=0; deviceIndex < inputDevicesEnabledStatuses.size(); deviceIndex++) {
        if(deviceIndex < midiStreams.size()){
            RtMidiIn* stream = midiStreams.at(deviceIndex);
            if(stream && inputDevicesEnabledStatuses.at(deviceIndex)){//device is globally enabled?
                if(!stream->isPortOpen()){
                    try{
                        qCInfo(jtMidi) << "Starting MIDI in " << QString::fromStdString(stream->getPortName(deviceIndex));
                        stream->openPort(deviceIndex);
                    }
                    catch(RtMidiError e){
                        qCCritical(jtMidi) << "Error opening midi port " << QString::fromStdString(e.getMessage());
                    }
                }
                else{
                    qCCritical(jtMidi) << "Port " << QString::fromStdString(stream->getPortName(deviceIndex)) << " already opened!";
                }
            }
        }
    }
}

void RtMidiDriver::stop(){
    foreach (RtMidiIn* stream, midiStreams) {
        if(stream){
            stream->closePort();
        }
    }
}

void RtMidiDriver::release(){
    foreach (RtMidiIn* stream, midiStreams) {
        if(stream){
            if(stream->isPortOpen()){
                stream->closePort();
            }
            delete stream;
        }
    }
    midiStreams.clear();
}

QString RtMidiDriver::getInputDeviceName(uint index) const{
    RtMidiIn rtMidi;
    if(index < rtMidi.getPortCount())
        return QString::fromStdString(rtMidi.getPortName(index));
    return "";
}

MidiBuffer RtMidiDriver::getBuffer(){
    MidiBuffer buffer(32);//max 32 midi messages
    int deviceIndex = 0;
    foreach (RtMidiIn* stream, midiStreams) {
        while(true){
            std::vector<unsigned char> message;
            double stamp = stream->getMessage(&message);
            if(!message.empty() && message.size() <= 3){
                int msgData = 0;
                msgData |= message.at(0);
                if(message.size() == 3){
                    msgData |= message.at(1) << 8;
                    msgData |= message.at(2) << 16;
                }else{
                    msgData = 0;
                    qCWarning(jtMidi) << "We receive a not common midi message with just" << message.size() << "bytes";
                }
                //qDebug(midi) << "original msgData:";
                //qDebug(midi) << "Status:" << message.at(0);
                //qDebug(midi) << "data 1:" << message.at(1);
                //qDebug(midi) << "data 2:" << message.at(2) << endl;

                buffer.addMessage(MidiMessage(msgData, qint32(stamp), deviceIndex));

                //            qDebug(midi) << "msgData:" << msgData;
                //            qDebug(midi) << "Status:" << ((msgData) & 0xFF);
                //            qDebug(midi) << "data 1:" << (((msgData) >> 8) & 0xFF);
                //            qDebug(midi) << "data 2:" << (((msgData) >> 16) & 0xFF);
            }
            else{
                break;
            }
        }
        deviceIndex++;
    }
    return buffer;
}

bool RtMidiDriver::hasInputDevices() const{
    return getMaxInputDevices() > 0;
}

int RtMidiDriver::getMaxInputDevices() const{
    RtMidiIn rtMidi;
    return rtMidi.getPortCount();
}

RtMidiDriver::~RtMidiDriver(){
    release();
}
