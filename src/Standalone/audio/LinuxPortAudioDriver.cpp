#include "PortAudioDriver.h"
#include "log/Logging.h"
#include "pa_linux_alsa.h"

using controller::MainController;

namespace audio {

PortAudioDriver::PortAudioDriver(MainController *mainController, QString audioInputDevice, QString audioOutputDevice,
                                 int firstInIndex, int lastInIndex, int firstOutIndex,
                                 int lastOutIndex, int sampleRate, int bufferSize) :
    AudioDriver(mainController),
    useSystemDefaultDevices(false)
{

    Q_UNUSED(firstInIndex)
    Q_UNUSED(firstOutIndex)
    Q_UNUSED(lastInIndex)
    Q_UNUSED(lastOutIndex)

    // initialize portaudio using default devices
    PaError error = Pa_Initialize();
    if (error == paNoError) {
        audioInputDeviceIndex = getDeviceIndexByName(audioInputDevice);
        audioOutputDeviceIndex = getDeviceIndexByName(audioOutputDevice);
        globalInputRange = ChannelRange(0, getMaxInputs());
        globalOutputRange = ChannelRange(0, 2);// 2 channels for output

        int maxOutputs = getMaxOutputs();
        if (maxOutputs > 1)
            globalOutputRange.setToStereo();
        if(!initPortAudio(sampleRate, bufferSize)){
            qCritical() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
            audioInputDeviceIndex = audioOutputDeviceIndex = paNoDevice;
        }
    } else {
        qCritical() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        audioInputDeviceIndex = audioOutputDeviceIndex = paNoDevice;
    }
}

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
