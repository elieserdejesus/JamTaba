#include "MidiMessage.h"

#include <QDebug>

using namespace Midi;

MidiMessage::MidiMessage(qint32 data, int sourceID)
    : data(data),
      sourceID(sourceID)
{

}


MidiMessage::MidiMessage()
    :data(-1),
     sourceID(-1)
{

}

MidiMessage::MidiMessage(const MidiMessage &other)
    : data(other.data),
      sourceID(other.sourceID)
{

}

MidiMessage MidiMessage::fromVector(std::vector<unsigned char> vector, qint32 deviceIndex)
{
    int msgData = 0;
    msgData |= vector.at(0);
    msgData |= vector.at(1) << 8;
    msgData |= vector.at(2) << 16;
    return Midi::MidiMessage(msgData, deviceIndex);
}

MidiMessage MidiMessage::fromArray(const char array[4], qint32 deviceIndex)
{
    std::vector<unsigned char> vector;
    vector.assign(array, array + 4);
    return fromVector(vector, deviceIndex);
}

// +++++++++++++++++++++++

void MidiMessage::transpose(qint8 semitones)
{
    if (!isNote() || semitones == 0) {
        return;
    }
    quint32 newValue = data + (semitones << 8);
    if ( ((newValue >> 8) & 0xFF) <= 127 ){
        data = newValue;
    }
}

quint8 MidiMessage::getNoteVelocity() const
{
    if (!isNote())
        return 0;
    return getData2();
}

bool MidiMessage::isNote() const
{
    //0x80 = Note Off
    //0x90 = Note On
    // 0x80 to 0x9F where the low nibble is the MIDI channel.
    int status = getStatus();
    return status >= 0x80 && status <= 0x9F;
}

bool MidiMessage::isNoteOn() const
{
    int status = getStatus();
    return status >= 0x90 && status <= 0x9F;
}

bool MidiMessage::isNoteOff() const
{
    int status = getStatus();
    return status >= 0x80 && status <= 0x8F;
}
