#include "MidiMessage.h"

using namespace Midi;

MidiMessage::MidiMessage(qint32 data, qint32 timestamp, int sourceDeviceIndex)
{
    this->data = data;
    this->timestamp = timestamp;
    this->deviceIndex = sourceDeviceIndex;
}

MidiMessage::MidiMessage()
{
    this->data = this->timestamp = this->deviceIndex = -1;
}

MidiMessage::MidiMessage(const MidiMessage &other)
{
    this->data = other.data;
    this->timestamp = other.timestamp;
    this->deviceIndex = other.deviceIndex;
}

// +++++++++++++++++++++++

quint8 MidiMessage::getNoteVelocity() const
{
    if (!isNote())
        return 0;
    return getData2();
}

bool MidiMessage::isNote() const
{
    // 0x90 to 0x9F where the low nibble is the MIDI channel.
    int status = getStatus();
    return status >= 0x90 && status <= 0x9F;
}
