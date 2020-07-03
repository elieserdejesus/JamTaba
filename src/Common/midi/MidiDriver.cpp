#include "MidiDriver.h"
#include "log/Logging.h"
#include "MidiMessage.h"

using midi::MidiDriver;

MidiDriver::MidiDriver()
{
}

MidiDriver::~MidiDriver()
{
}

int MidiDriver::getFirstGloballyEnableInputDevice() const
{
    int total = getMaxInputDevices();
    for (int i = 0; i < total; ++i) {
        if (deviceIsGloballyEnabled(i))
            return i;
    }
    return -1;
}

void MidiDriver::setDevicesStatus(const QList<bool> &inputStatuses)
{
    this->inputDevicesEnabledStatuses = inputStatuses;
}

bool MidiDriver::deviceIsGloballyEnabled(int deviceIndex) const
{
    if (deviceIndex >= 0 && deviceIndex < inputDevicesEnabledStatuses.size())
        return inputDevicesEnabledStatuses.at(deviceIndex);

    return false;
}
