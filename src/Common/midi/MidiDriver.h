#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>

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

class MidiBuffer
{
public:
    explicit MidiBuffer(int maxMessages);
    ~MidiBuffer();
    void addMessage(const MidiMessage &m);
    MidiMessage getMessage(int index) const;
    int getMessagesCount() const
    {
        return messagesCount;
    }

    MidiBuffer(const MidiBuffer &other);
private:
    int maxMessages;
    MidiMessage *messages;
    int messagesCount;
};

class MidiDriver
{
public:
    MidiDriver();
    virtual ~MidiDriver();

    virtual void start(const QList<bool> &deviceStatuses) = 0;
    virtual void stop() = 0;
    virtual void release() = 0;

    virtual bool hasInputDevices() const = 0;

    virtual int getMaxInputDevices() const = 0;

    virtual QString getInputDeviceName(int index) const = 0;
    virtual MidiBuffer getBuffer() = 0;

    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;
    int getFirstGloballyEnableInputDevice() const;
    virtual void setInputDevicesStatus(QList<bool> statuses);

protected:
    QList<bool> inputDevicesEnabledStatuses;// store the globally enabled midi input devices
    int selectedChannel;// -1 to use all channels
};

class NullMidiDriver : public MidiDriver
{
    inline virtual void start(const QList<bool> &deviceStatuses) override
    {
    }

    inline virtual void stop() override
    {
    }

    inline virtual void release() override
    {
    }

    inline virtual bool hasInputDevices() const override
    {
        return false;
    }

    inline virtual int getMaxInputDevices() const override
    {
        return 0;
    }

    inline virtual QString getInputDeviceName(int index) const override
    {
        Q_UNUSED(index);
        return "";
    }

    inline virtual MidiBuffer getBuffer() override
    {
        return MidiBuffer(0);
    }
};
}

#endif // MIDIDRIVER_H
