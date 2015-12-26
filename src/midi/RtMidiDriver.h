#ifndef RTMIDIDRIVER_H
#define RTMIDIDRIVER_H

#include "MidiDriver.h"
#include "RtMidi.h"

namespace Midi {
class RtMidiDriver : public MidiDriver
{
public:
    RtMidiDriver(QList<bool> deviceStatuses);
    ~RtMidiDriver();

    virtual void start();
    virtual void stop();
    virtual void release();

    virtual bool hasInputDevices() const;
    virtual int getMaxInputDevices() const;
    virtual QString getInputDeviceName(int index) const;
    virtual MidiBuffer getBuffer();

    virtual void setInputDevicesStatus(QList<bool> statuses);

private:
    QList<RtMidiIn *> midiStreams;
};
}
#endif // RTMIDIDRIVER_H
