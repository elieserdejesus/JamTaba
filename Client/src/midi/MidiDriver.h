#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>

namespace Midi {

class MidiMessage{
public:
    MidiMessage(qint32 data, qint32 timestamp, int sourceDeviceIndex);
    MidiMessage();
    MidiMessage(const MidiMessage& other);

    inline int getChannel() const{ return data & 0x0000000F; }

    inline int getDeviceIndex() const{return deviceIndex;}

    inline int getStatus() const{ return data & 0xFF;}
    inline int getData1() const{ return (data >> 8) & 0xFF;}
    inline int getData2() const{ return (data >> 16) & 0xFF;}
    
    //MIDI Control Change
    inline bool isControl() const{return getStatus() == 0xB0;}
    //type=0 to 127
    inline int getControlType() const{return getData1();}
    //value=0 to 127
    inline int getControlVal() const{return getData2();}
    
private:
    qint32 data;
    qint32 timestamp;
    int deviceIndex;
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
    //int writeIndex;
    //int readIndex;
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

    virtual QString getInputDeviceName(int index) const = 0;
    virtual MidiBuffer getBuffer() = 0;


    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;
    int getFirstGloballyEnableInputDevice() const;
    virtual void setInputDevicesStatus(QList<bool> statuses);

protected:
    QList<bool> inputDevicesEnabledStatuses;//stode the globally enabled midi input devices
    int selectedChannel;//-1 to use all channels
};

class NullMidiDriver : public MidiDriver{
    inline virtual void start() {}
    inline virtual void stop() {}
    inline virtual void release(){}

    inline virtual bool hasInputDevices() const{return false;}

    inline virtual int getMaxInputDevices() const {return 0;}

    inline virtual QString getInputDeviceName(int index) const{Q_UNUSED(index); return "";}
    inline virtual MidiBuffer getBuffer(){return MidiBuffer(0);}
};

}

#endif // MIDIDRIVER_H
