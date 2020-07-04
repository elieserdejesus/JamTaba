#ifndef RTMIDIDRIVER_H
#define RTMIDIDRIVER_H

#include "MidiDriver.h"
#include "RtMidi.h"

namespace midi {

class RtMidiDriver : public MidiDriver
{
public:
    explicit RtMidiDriver(const QList<bool> &inputDeviceStatuses, const QList<bool> &outputDeviceStatuses);
    ~RtMidiDriver();

    void start(const QList<bool> &inputDeviceStatuses,const QList<bool> &outputDeviceStatuses) override;
    void stop() override;
    void release() override;

    void setDevicesStatus(const QList<bool> &inputStatuses, const QList<bool> &outputStatuses) override;

    bool hasInputDevices() const override;
    bool hasOutputDevices() const override;
    int getMaxInputDevices() const override;
    int getMaxOutputDevices() const override;
    QString getInputDeviceName(uint index) const override;
    QString getOutputDeviceName(uint index) const override;
    std::vector<midi::MidiMessage> getBuffer() override;

    void sendClockStart() const override;
    void sendClockStop() const override;
    void sendClockContinue() const override;
    void sendClockPulse() const override;

private:
    QList<RtMidiIn *> midiInStreams;
    QList<RtMidiOut *> midiOutStreams;

    void consumeMessagesFromStream(RtMidiIn *stream, int deviceIndex, std::vector<MidiMessage> &outBuffer);

};
}
#endif // RTMIDIDRIVER_H
