#include "MidiDriver.h"
#include "log/Logging.h"
#include "MidiMessage.h"

using namespace Midi;
// +++++++++++++++++++++++

// +++++++++++++++++++++++++++++++++++++

MidiMessageBuffer::MidiMessageBuffer(int maxMessages) :
    maxMessages(maxMessages),
    messages(new MidiMessage[maxMessages]),
    messagesCount(0)
{
}

MidiMessageBuffer::MidiMessageBuffer(const MidiMessageBuffer &other) :
    maxMessages(other.maxMessages),
    messages(new MidiMessage[other.maxMessages]),
    messagesCount(other.messagesCount)
{
    for (int m = 0; m < other.messagesCount; ++m)
        this->messages[m] = other.messages[m];
}

MidiMessageBuffer::~MidiMessageBuffer()
{
    delete [] messages;
}

void MidiMessageBuffer::addMessage(const MidiMessage &m)
{
    if (messagesCount < maxMessages) {
        messages[messagesCount] = m;
        messagesCount++;
    } else {
        qWarning() << "MidiBuffer full, discarding the message!";
    }
}

MidiMessage MidiMessageBuffer::getMessage(int index) const
{
    if (index >= 0 && index < messagesCount)
        return messages[index];
    return MidiMessage();
}

// ++++++++++++++++
MidiDriver::MidiDriver()
{
}

MidiDriver::~MidiDriver()
{
}

int MidiDriver::getFirstGloballyEnableInputDevice() const
{
    int total = getMaxInputDevices();
    for (int i = 0; i < total; ++i) {
        if (deviceIsGloballyEnabled(i))
            return i;
    }
    return -1;
}

void MidiDriver::setInputDevicesStatus(const QList<bool> &statuses)
{
    this->inputDevicesEnabledStatuses = statuses;
}

bool MidiDriver::deviceIsGloballyEnabled(int deviceIndex) const
{
    if (deviceIndex >= 0 && deviceIndex < inputDevicesEnabledStatuses.size())
        return inputDevicesEnabledStatuses.at(deviceIndex);
    return false;
}
