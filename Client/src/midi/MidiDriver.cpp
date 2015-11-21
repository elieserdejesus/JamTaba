#include "MidiDriver.h"
#include "../log/logging.h"

using namespace Midi;

MidiMessage::MidiMessage(qint32 data, qint32 timestamp, int sourceDeviceIndex)
    :data(data), timestamp(timestamp), deviceIndex(sourceDeviceIndex){

}

MidiMessage::MidiMessage()
    :data(-1), timestamp(-1), deviceIndex(-1){

}

//this copy constructor is really necessary? The sky is really blue? I don't know ... :)
MidiMessage::MidiMessage(const MidiMessage &other)
    : data(other.data), timestamp(other.timestamp), deviceIndex(other.deviceIndex){

}

quint8 MidiMessage::getNoteVelocity() const{
    if(!isNote()){
        return 0;
    }
    return getData2();
}

bool MidiMessage::isNote() const{
    //0x90 to 0x9F where the low nibble is the MIDI channel.
    int status = getStatus();
    return status >= 0x90 && status <= 0x9F;
}

//+++++++++++++++++++++++++++++++++++++
MidiBuffer::MidiBuffer(int maxMessages)
    : maxMessages(maxMessages),
      messages(new MidiMessage[maxMessages]),
      //writeIndex(0), readIndex(0),
      messagesCount(0)
{

}

MidiBuffer::MidiBuffer(const MidiBuffer &other)
    : maxMessages(other.maxMessages),
      messages(new MidiMessage[other.maxMessages]),
      //writeIndex(other.writeIndex), readIndex(other.readIndex),
      messagesCount(other.messagesCount)
{
    for (int m = 0; m < other.messagesCount; ++m) {
        this->messages[m] = other.messages[m];
    }
}

MidiBuffer::~MidiBuffer(){
    delete [] messages;
}

void MidiBuffer::addMessage(const MidiMessage &m){
    if(messagesCount < maxMessages){
        messages[messagesCount] = m;
        messagesCount++;
    }
    else{
        qCWarning(jtMidi) << "MidiBuffer full, discarding the message!";
    }
}

MidiMessage MidiBuffer::getMessage(int index) const{
    if(index >= 0 && index < messagesCount){
        return messages[index];
    }
    return MidiMessage();
}

//++++++++++++++++
MidiDriver::MidiDriver()
{

}

MidiDriver::~MidiDriver()
{

}

int MidiDriver::getFirstGloballyEnableInputDevice() const{
    int total = getMaxInputDevices();
    for (int i = 0; i < total; ++i) {
        if(deviceIsGloballyEnabled(i)){
            return i;
        }
    }
    return -1;
}

void MidiDriver::setInputDevicesStatus(QList<bool> statuses){
    this->inputDevicesEnabledStatuses = statuses;
}

bool MidiDriver::deviceIsGloballyEnabled(int deviceIndex) const{
    if(deviceIndex >= 0 && deviceIndex < inputDevicesEnabledStatuses.size()){
        return inputDevicesEnabledStatuses.at(deviceIndex);
    }
    return false;
}
