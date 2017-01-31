#include "MidiDriver.h"
#include "log/Logging.h"
#include "MidiMessage.h"

using namespace Midi;

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

void MidiDriver::setInputDevicesStatus(const QList<bool> &statuses)
{
    this->inputDevicesEnabledStatuses = statuses;
}

bool MidiDriver::deviceIsGloballyEnabled(int deviceIndex) const
{
    if (deviceIndex >= 0 && deviceIndex < inputDevicesEnabledStatuses.size())
        return inputDevicesEnabledStatuses.at(deviceIndex);
    return false;
}
