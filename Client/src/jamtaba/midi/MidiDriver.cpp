#include "MidiDriver.h"

using namespace Midi;

MidiBuffer::MidiBuffer(int maxMessages)
    :maxMessages(maxMessages){
    messages = new MidiMessage[maxMessages];
    writeIndex = readIndex = 0;
    messagesCount = 0;
}

MidiBuffer::~MidiBuffer(){
    delete [] messages;
}

void MidiBuffer::addMessage(const MidiMessage &m){
    messages[writeIndex].data = m.data;
    messages[writeIndex].timestamp = m.timestamp;
    writeIndex = (writeIndex + 1) % maxMessages;
    messagesCount++;
}

MidiMessage MidiBuffer::consumeMessage(){
    if(messagesCount > 0){
        messagesCount--;
        return messages[readIndex++];
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

