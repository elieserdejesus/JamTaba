#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>
#include "MidiMessageBuffer.h"

namespace Midi {

class MidiMessage;

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

    virtual QString getInputDeviceName(uint index) const = 0;
    virtual MidiMessageBuffer getBuffer() = 0;

    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;
    int getFirstGloballyEnableInputDevice() const;
    virtual void setInputDevicesStatus(const QList<bool> &statuses);

protected:
    QList<bool> inputDevicesEnabledStatuses;// store the globally enabled midi input devices
    int selectedChannel;// -1 to use all channels
};

class NullMidiDriver : public MidiDriver
{
    inline virtual void start(const QList<bool> &deviceStatuses) override
    {
        Q_UNUSED(deviceStatuses)
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

    inline virtual QString getInputDeviceName(uint index) const override
    {
        Q_UNUSED(index);
        return "";
    }

    inline virtual MidiMessageBuffer getBuffer() override
    {
        return MidiMessageBuffer(0);
    }
};
}

#endif // MIDIDRIVER_H
