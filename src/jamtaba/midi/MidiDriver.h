#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>

namespace Midi {


struct MidiMessage{
    qint32 data;
    qint32 timestamp;

    MidiMessage(qint32 data, qint32 timestamp){
        this->data = data;
        this->timestamp = timestamp;
    }

    MidiMessage(){
        this->data = this->timestamp = 0;
    }
};

class MidiBuffer{
public:
    MidiBuffer(int maxMessages);
    ~MidiBuffer();
    void addMessage(const MidiMessage& m);
    MidiMessage consumeMessage();
    int getMessagesCount() const {return messagesCount;}
private:
    int maxMessages;
    MidiMessage* messages;
    int writeIndex;
    int readIndex;
    int messagesCount;
};

class MidiDriver
{
public:
    MidiDriver();
    virtual ~MidiDriver();

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void release() = 0;
    //virtual void initialize() = 0;

    virtual int getMaxInputDevices() const = 0;
    virtual const char* getInputDeviceName(int index) const = 0;
    virtual MidiBuffer getBuffer() = 0;
    virtual int getInputDeviceIndex() const = 0;
    virtual void setInputDeviceIndex(int index) = 0;

    /*

    //virtual int getMaxInputs() const = 0;

    //virtual int getMaxOutputs() const = 0;

    //virtual int getInputs() const = 0;
    //virtual int getFirstInput() const = 0;
    //virtual int getOutputs() const = 0;
    //virtual int getFirstOutput() const = 0;



    //virtual int getOutputDeviceIndex() const = 0;
    //virtual void setOutputDeviceIndex(int index) = 0;

    virtual const char* getInputDeviceName(int index) const = 0;
    virtual const char* getOutputDeviceName(int index) const = 0;
    virtual int getDevicesCount() const = 0;

    virtual const char* getInputChannelName(unsigned const int index) const = 0;
    virtual const char* getOutputChannelName(unsigned const int index) const = 0;

    virtual int getSampleRate() const = 0;
    virtual int getBufferSize() const = 0;
    */
};

}

#endif // MIDIDRIVER_H
