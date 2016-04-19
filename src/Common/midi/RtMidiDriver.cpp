#include "RtMidiDriver.h"
#include "RtMidi.h"

#include "MidiMessage.h"

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
                        stream->ignoreTypes();// ignoring sysex, miditime and midi sense messages
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

void RtMidiDriver::consumeMessagesFromStream(RtMidiIn *stream, int deviceIndex, MidiMessageBuffer &outBuffer)
{
    std::vector<unsigned char> messageBytes;
    do{
        messageBytes.clear();
        double messageTimeStamp = stream->getMessage(&messageBytes);
        if (messageBytes.size() == 3) { // Jamtaba is handling only the 3 bytes commond midi messages. Uncommon midi messages will be ignored.
            int msgData = 0;
            msgData |= messageBytes.at(0);
            msgData |= messageBytes.at(1) << 8;
            msgData |= messageBytes.at(2) << 16;
            outBuffer.addMessage(Midi::MidiMessage(msgData, qint32(messageTimeStamp), deviceIndex));
        }
        else{
            if (!messageBytes.empty())
                qWarning() << "A midi message containing " << messageBytes.size() << " bytes was received!";
        }
    }
    while(!messageBytes.empty());
}

MidiMessageBuffer RtMidiDriver::getBuffer(){
    MidiMessageBuffer buffer(64);//max 64 midi messages in each audio callback
    int deviceIndex = 0;
    foreach (RtMidiIn* stream, midiStreams) {
        consumeMessagesFromStream(stream, deviceIndex, buffer);
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
    //release();
}
