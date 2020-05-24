#include "PortAudioDriver.h"
#include "pa_mac_core.h"
#include "log/Logging.h"

namespace audio {

QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const
{
    QList<int> buffersSize;
    long maxBufferSize;
    long minBufferSize;
    PaError result = PaMacCore_GetBufferSizeRange(deviceIndex, &minBufferSize, &maxBufferSize);
    if (result != paNoError) {
        buffersSize.append(256);
        return buffersSize;// return 256 as the only possible value
    }
    for (long size = minBufferSize; size <= maxBufferSize; size *= 2)
        buffersSize.append(size);
    return buffersSize;
}

QString PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    PaDeviceIndex device = Pa_GetDefaultOutputDevice();
    if (device == paNoDevice)
        return "Error! No Output!";

    return QString(PaMacCore_GetChannelName(device, index, false));
}

QString PortAudioDriver::getInputChannelName(const unsigned int index) const
{
    PaDeviceIndex device = Pa_GetDefaultInputDevice();
    if (device == paNoDevice)
        return "Error! No Input!";

    return QString(PaMacCore_GetChannelName(device, index, true));
}

void PortAudioDriver::configureHostSpecificInputParameters(PaStreamParameters &inputParameters)
{
    Q_UNUSED(inputParameters)
    // qCDebug(portaudio) << "using MAC scpecific stream infos for inputs";
    // PaMacCoreStreamInfo streamInfo;
    // PaMacCore_SetupStreamInfo(&streamInfo, paMacCorePro);
    // inputParams.hostApiSpecificStreamInfo  = &streamInfo;
}

void PortAudioDriver::configureHostSpecificOutputParameters(PaStreamParameters &outputParameters)
{
    Q_UNUSED(outputParameters)
    // qCDebug(portaudio) << "using MAC scpecific stream infos for output";
    // PaMacCoreStreamInfo outStreamInfo;
    // PaMacCore_SetupStreamInfo(&outStreamInfo, paMacCorePro);
    // outputParams.hostApiSpecificStreamInfo = &outStreamInfo;
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

void PortAudioDriver::preInitializePortAudioStream(PaStream *stream)
{
    Q_UNUSED(stream)
}


}// namespace
