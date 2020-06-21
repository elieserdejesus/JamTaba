#ifndef RTMIDIDRIVER_H
#define RTMIDIDRIVER_H

#include "MidiDriver.h"
#include "RtMidi.h"

namespace midi {

class RtMidiDriver : public MidiDriver
{
public:
    explicit RtMidiDriver(const QList<bool> &deviceStatuses);
    ~RtMidiDriver();

    void start(const QList<bool> &deviceStatuses) override;
    void stop() override;
    void release() override;

    void setInputDevicesStatus(const QList<bool> &statuses) override;

    bool hasInputDevices() const override;
    bool hasOutputDevices() const override;
    int getMaxInputDevices() const override;
    int getMaxOutputDevices() const override;
    QString getInputDeviceName(uint index) const override;
    QString getOutputDeviceName(uint index) const override;
    std::vector<midi::MidiMessage> getBuffer() override;

    void sendClockStart() const override;
    void sendClockTick() const override;

private:
    QList<RtMidiIn *> midiInStreams;
    QList<RtMidiOut *> midiOutStreams;

    void consumeMessagesFromStream(RtMidiIn *stream, int deviceIndex, std::vector<MidiMessage> &outBuffer);

};
}
#endif // RTMIDIDRIVER_H
