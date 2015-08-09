#include "MidiDriver.h"

using namespace Midi;

MidiBuffer::MidiBuffer(int maxMessages)
    : maxMessages(maxMessages),
      messages(new MidiMessage[maxMessages]),
      writeIndex(0), readIndex(0),
      messagesCount(0)
{

}

MidiBuffer::MidiBuffer(const MidiBuffer &other)
    : maxMessages(other.maxMessages),
      messages(new MidiMessage[other.maxMessages]),
      writeIndex(other.writeIndex), readIndex(other.readIndex),
      messagesCount(other.messagesCount)
{
    for (int m = 0; m < other.messagesCount; ++m) {
        this->messages[m].data = other.messages[m].data;
        this->messages[m].timestamp = other.messages[m].timestamp;
    }
}

MidiBuffer::~MidiBuffer(){
    delete [] messages;
}

void MidiBuffer::addMessage(const MidiMessage &m){
    messages[writeIndex].data = m.data;
    messages[writeIndex].timestamp = m.timestamp;
    messages[writeIndex].globalSourceDeviceIndex = m.globalSourceDeviceIndex;
    writeIndex = (writeIndex + 1) % maxMessages;
    messagesCount++;
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
