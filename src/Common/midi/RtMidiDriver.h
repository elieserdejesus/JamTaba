#ifndef RTMIDIDRIVER_H
#define RTMIDIDRIVER_H

#include "MidiDriver.h"
#include "RtMidi.h"

namespace Midi {
class RtMidiDriver : public MidiDriver
{
public:
    RtMidiDriver(const QList<bool> &deviceStatuses);
    ~RtMidiDriver();

    void start(const QList<bool> &deviceStatuses) override;
    void stop() override;
    void release() override;

    void setInputDevicesStatus(const QList<bool> &statuses) override;

    bool hasInputDevices() const override;
    int getMaxInputDevices() const override;
    QString getInputDeviceName(uint index) const override;
    MidiMessageBuffer getBuffer() override;

private:
    QList<RtMidiIn *> midiStreams;

    void consumeMessagesFromStream(RtMidiIn *stream, int deviceIndex, MidiMessageBuffer &outBuffer);

};
}
#endif // RTMIDIDRIVER_H
