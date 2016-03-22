#ifndef _MIDI_MESSAGE_
#define _MIDI_MESSAGE_

#include <QtGlobal>

namespace Midi {

class MidiMessage
{
public:
    MidiMessage(qint32 data, qint32 timestamp, int sourceDeviceIndex);
    MidiMessage();
    MidiMessage(const MidiMessage &other);

    inline int getChannel() const
    {
        return data & 0x0000000F;
    }

    bool isNote() const;

    void transpose(qint8 semitones);

    quint8 getNoteVelocity() const;

    inline int getDeviceIndex() const
    {
        return deviceIndex;
    }

    inline int getStatus() const
    {
        return data & 0xFF;
    }

    inline int getData1() const
    {
        return (data >> 8) & 0xFF;
    }

    inline int getData2() const
    {
        return (data >> 16) & 0xFF;
    }

    inline bool isControl() const
    {
        return getStatus() == 0xB0;
    }

private:
    qint32 data;
    qint32 timestamp;
    int deviceIndex;
};

}//namespace

#endif
