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

    void start(const QList<bool> &deviceStatuses) override;
    void stop() override;
    void release() override;

    bool hasInputDevices() const override;
    int getMaxInputDevices() const override;
    QString getInputDeviceName(int index) const override;
    MidiBuffer getBuffer() override;

private:
    QList<RtMidiIn *> midiStreams;
    void setInputDevicesStatus(const QList<bool> &statuses);
};
}
#endif // RTMIDIDRIVER_H
