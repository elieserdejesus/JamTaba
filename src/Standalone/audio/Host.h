#ifndef HOST_H
#define HOST_H

#include <QList>
#include "midi/MidiMessage.h"

class Host
{

public:
    //Host(){}
    virtual ~Host(){}
    virtual int getSampleRate() const = 0;
    virtual int getBufferSize() const = 0;

    virtual std::vector<Midi::MidiMessage> pullReceivedMidiMessages() = 0;

    virtual void setSampleRate(int sampleRate) = 0;
    virtual void setBlockSize(int blockSize) = 0;
    virtual void setTempo(int bpm) = 0;
    virtual void setPlayingFlag(bool playing) = 0;
    virtual void setPositionInSamples(int position) = 0;

protected:
    std::vector<Midi::MidiMessage> receivedMidiMessages;

};

#endif // HOST_H
