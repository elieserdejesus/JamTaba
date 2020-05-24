#include "PortAudioDriver.h"
#include "log/Logging.h"
#include "pa_linux_alsa.h"

using controller::MainController;

namespace audio {

void PortAudioDriver::preInitializePortAudioStream(PaStream *stream)
{
    qCDebug(jtAudio) << "Enablind Realtime Scheduling in ALSA";
    PaAlsa_EnableRealtimeScheduling(stream, 1);// enable realtime scheduling in ALSA

}

QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const
{
    Q_UNUSED(deviceIndex);

    int value = 32;
    QList<int> bufferSizes;
    while (value <= 4096) {
        bufferSizes.append(value);
        value *= 2;
    }
    return bufferSizes;
}

QString PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    auto deviceInfo = Pa_GetDeviceInfo(audioOutputDeviceIndex);
    if (index < (uint)deviceInfo->maxOutputChannels)
        return "Out " + QString::number(index + 1);
    return "error";
}

QString PortAudioDriver::getInputChannelName(const unsigned int index) const
{
    auto deviceInfo = Pa_GetDeviceInfo(audioInputDeviceIndex);
    if (index < (uint)deviceInfo->maxInputChannels)
        return "In " + QString::number(index + 1);
    return "error";
}

void PortAudioDriver::configureHostSpecificInputParameters(PaStreamParameters &inputParameters)
{
    Q_UNUSED(inputParameters)

}

void PortAudioDriver::configureHostSpecificOutputParameters(PaStreamParameters &outputParameters)
{
    Q_UNUSED(outputParameters)

}

void PortAudioDriver::releaseHostSpecificParameters(const PaStreamParameters &inputParameters,
                                                    const PaStreamParameters &outputParameters)
{
    Q_UNUSED(inputParameters)
    Q_UNUSED(outputParameters)
}

bool PortAudioDriver::hasControlPanel() const
{
    return false;
}

void PortAudioDriver::openControlPanel(void *mainWindowHandle)
{
    Q_UNUSED(mainWindowHandle)
}
}// namespace
