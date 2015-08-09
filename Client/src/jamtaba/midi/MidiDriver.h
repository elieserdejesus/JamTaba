#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>

namespace Midi {


struct MidiMessage{
    qint32 data;
    qint32 timestamp;
    int globalSourceDeviceIndex;

    MidiMessage(qint32 data, qint32 timestamp, int sourceDeviceIndex){
        this->data = data;
        this->timestamp = timestamp;
        this->globalSourceDeviceIndex = sourceDeviceIndex;
    }

    MidiMessage(){
        this->data = this->timestamp = this->globalSourceDeviceIndex = 0;
    }
};

class MidiBuffer{
public:
    explicit MidiBuffer(int maxMessages);
    ~MidiBuffer();
    void addMessage(const MidiMessage& m);
    //MidiMessage consumeMessage();
    MidiMessage getMessage(int index) const;
    int getMessagesCount() const {return messagesCount;}
    MidiBuffer(const MidiBuffer& other);
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

    virtual bool hasInputDevices() const = 0;

    virtual int getMaxInputDevices() const = 0;

    virtual const char* getInputDeviceName(int index) const = 0;
    virtual MidiBuffer getBuffer() = 0;
    //virtual int getInputDeviceIndex() const = 0;
    //virtual void setInputDeviceIndex(int deviceIndex) = 0;

    virtual void setDeviceGlobalEnabledStatus(int deviceIndex, bool enabled);
    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;

protected:
    QMap<int, bool> inputDevicesEnabledStatuses;//stode the globally enabled midi input devices
};

}

#endif // MIDIDRIVER_H
