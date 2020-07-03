#ifndef MIDIDRIVER_H
#define MIDIDRIVER_H

#include <QtGlobal>
#include <QMap>

#include "MidiMessage.h"

#include <vector>

namespace midi {


class MidiDriver
{

public:
    MidiDriver();
    virtual ~MidiDriver();

    virtual void start(const QList<bool> &deviceStatuses) = 0;
    virtual void stop() = 0;
    virtual void release() = 0;

    virtual bool hasInputDevices() const = 0;
    virtual bool hasOutputDevices() const = 0;

    virtual int getMaxInputDevices() const = 0;
    virtual int getMaxOutputDevices() const = 0;

    virtual QString getInputDeviceName(uint index) const = 0;
    virtual QString getOutputDeviceName(uint index) const = 0;

    virtual std::vector<MidiMessage> getBuffer() = 0;

    virtual bool deviceIsGloballyEnabled(int deviceIndex) const;
    int getFirstGloballyEnableInputDevice() const;
    virtual void setInputDevicesStatus(const QList<bool> &statuses);

    virtual void sendClockStart() const = 0;
    virtual void sendClockStop() const = 0;
    virtual void sendClockPulse() const = 0;

protected:
    QList<bool> inputDevicesEnabledStatuses; // store the globally enabled midi input devices
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

    inline virtual bool hasOutputDevices() const override
    {
        return false;
    }

    inline virtual int getMaxInputDevices() const override
    {
        return 0;
    }

    inline virtual int getMaxOutputDevices() const override
    {
        return 0;
    }

    inline virtual QString getInputDeviceName(uint index) const override
    {
        Q_UNUSED(index);
        return "";
    }

    inline virtual QString getOutputDeviceName(uint index) const override
    {
        Q_UNUSED(index);
        return "";
    }

    inline std::vector<MidiMessage> getBuffer() override
    {
        return std::vector<MidiMessage>();
    }

    void sendClockStart() const override
    {
    }

    void sendClockStop() const override
    {
    }

    void sendClockPulse() const override
    {
    }
};

} // namespace

#endif // MIDIDRIVER_H
