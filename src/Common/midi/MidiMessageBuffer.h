#ifndef _MIDI_MESSAGE_BUFFER_
#define _MIDI_MESSAGE_BUFFER_

#include <QList>

namespace Midi {

class MidiMessage;

class MidiMessageBuffer
{
public:
    explicit MidiMessageBuffer(int maxMessages);
    ~MidiMessageBuffer();

    void addMessage(const MidiMessage &m);

    MidiMessage &getMessage(int index) const;

    int getMessagesCount() const;

    bool isEmpty() const;

    QList<Midi::MidiMessage> toList() const;

    MidiMessageBuffer(const MidiMessageBuffer &other);

private:
    int maxMessages;
    MidiMessage *messages;
    int messagesCount;
};

inline bool MidiMessageBuffer::isEmpty() const
{
    return messagesCount <= 0;
}

inline int MidiMessageBuffer::getMessagesCount() const
{
    return messagesCount;
}

}//namespace

#endif
