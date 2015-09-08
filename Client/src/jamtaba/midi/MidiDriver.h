#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(portmidi)

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

    inline int getChannel() const{
        return data & 0x0000000F;
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

    //virtual void setSelectedChannel(int globalDeviceID, int channelIndex);
    //virtual void useAllChannels(int globalDeviceID);

    virtual const char* getInputDeviceName(int index) const = 0;
    virtual MidiBuffer getBuffer() = 0;


    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;
    int getFirstGloballyEnableInputDevice() const;
    virtual void setInputDevicesStatus(QList<bool> statuses);

protected:
    QList<bool> inputDevicesEnabledStatuses;//stode the globally enabled midi input devices
    int selectedChannel;//-1 to use all channels
};

class NullMidiDriver : public MidiDriver{
    virtual void start() {}
    virtual void stop() {}
    virtual void release(){}

    virtual bool hasInputDevices() const{return false;}

    virtual int getMaxInputDevices() const {return 0;}

    virtual const char* getInputDeviceName(int index) const{Q_UNUSED(index); return "";}
    virtual MidiBuffer getBuffer(){return MidiBuffer(0);}
};

}

#endif // MIDIDRIVER_H
