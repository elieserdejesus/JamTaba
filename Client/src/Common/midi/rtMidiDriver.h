#ifndef RTMIDIDRIVER_H
#define RTMIDIDRIVER_H

#include "MidiDriver.h"

#pragma warning(push)
#pragma warning(disable: 4100) //Unreferenced formal parameter

#include "RtMidi.h"

#pragma warning(pop)

namespace Midi {
class RtMidiDriver : public MidiDriver
{
public:
    RtMidiDriver(const QList<bool> &deviceStatuses);
    ~RtMidiDriver();

    virtual void start();
    virtual void stop();
    virtual void release();

    virtual bool hasInputDevices() const;
    virtual int getMaxInputDevices() const;
    virtual QString getInputDeviceName(int index) const;
    virtual MidiBuffer getBuffer();

    virtual void setInputDevicesStatus(const QList<bool> &statuses);

private:
    QList<RtMidiIn *> midiStreams;
};
}
#endif // RTMIDIDRIVER_H
