#ifndef PORTMIDIDRIVER_H
#define PORTMIDIDRIVER_H

#include "MidiDriver.h"
#include "portmidi.h"
#include <QList>

namespace Midi {

class PortMidiDriver : public MidiDriver
{
public:
    PortMidiDriver();
    ~PortMidiDriver();

    // MidiDriver interface
public:
    virtual void start();
    virtual void stop();
    virtual void release();

    virtual bool hasInputDevices() const;
    virtual int getMaxInputDevices() const;
    virtual const char *getInputDeviceName(int index) const;
    virtual MidiBuffer getBuffer();
    //virtual int getInputDeviceIndex() const;
    //virtual void setInputDeviceIndex(int index);

private:
    //PmDeviceID deviceId;
    //PmStream* stream;
    QList<PmStream*> streams;

    int getDeviceIDFromGlobalIndex(int globalIndex);
};

}

#endif // PORTMIDIDRIVER_H
