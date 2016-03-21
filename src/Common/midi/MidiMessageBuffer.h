#ifndef _MIDI_MESSAGE_BUFFER_
#define _MIDI_MESSAGE_BUFFER_

namespace Midi {

class MidiMessage;

class MidiMessageBuffer
{
public:
    explicit MidiMessageBuffer(int maxMessages);
    ~MidiMessageBuffer();
    void addMessage(const MidiMessage &m);
    MidiMessage getMessage(int index) const;
    int getMessagesCount() const
    {
        return messagesCount;
    }

    MidiMessageBuffer(const MidiMessageBuffer &other);
private:
    int maxMessages;
    MidiMessage *messages;
    int messagesCount;
};

}//namespace

#endif
