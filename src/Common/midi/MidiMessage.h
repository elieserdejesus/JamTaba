#ifndef _MIDI_MESSAGE_
#define _MIDI_MESSAGE_

#include <QtGlobal>
#include <vector>

namespace Midi {

class MidiMessage
{

public:
    MidiMessage(qint32 data, int sourceID);
    MidiMessage();

    static MidiMessage fromVector(std::vector<unsigned char> vector, qint32 sourceID);
    static MidiMessage fromArray(const char array[4], qint32 sourceID=-1);

    int getChannel() const;

    bool isNote() const;
    bool isNoteOn() const;
    bool isNoteOff() const;

    void transpose(qint8 semitones);

    quint8 getNoteVelocity() const;

    int getSourceDeviceIndex() const;

    int getStatus() const;

    int getData1() const;

    int getData2() const;

    bool isControl() const;

private:
    qint32 data;
    int sourceID; // the id of the midi device generating the message.
};

inline int MidiMessage::getChannel() const
{
    return data & 0x0000000F;
}

inline int MidiMessage::getSourceDeviceIndex() const
{
    return sourceID;
}

inline int MidiMessage::getStatus() const
{
    return data & 0xFF;
}

inline int MidiMessage::getData1() const
{
    return (data >> 8) & 0xFF;
}

inline int MidiMessage::getData2() const
{
    return (data >> 16) & 0xFF;
}

inline bool MidiMessage::isControl() const
{
    return getStatus() == 0xB0;
}

} // namespace

#endif
