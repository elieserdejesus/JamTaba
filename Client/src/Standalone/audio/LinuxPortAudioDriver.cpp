#include "PortAudioDriver.h"
#include "../log/Logging.h"

namespace Audio {

PortAudioDriver::PortAudioDriver(Controller::MainController *mainController, int deviceIndex,
                                 int firstInIndex, int lastInIndex, int firstOutIndex,
                                 int lastOutIndex, int sampleRate, int bufferSize) :
    AudioDriver(mainController)
{

    Q_UNUSED(firstInIndex)
    Q_UNUSED(firstOutIndex)
    Q_UNUSED(lastInIndex)
    Q_UNUSED(lastOutIndex)
    Q_UNUSED(deviceIndex)
    // initialize portaudio using default devices, mono input and try estereo output if possible
    PaError error = Pa_Initialize();
    if (error == paNoError) {
        audioDeviceIndex = Pa_GetDefaultOutputDevice();
        globalInputRange = ChannelRange(0, getMaxInputs());
        globalOutputRange = ChannelRange(0, 2);// 2 channels for output

        int maxOutputs = getMaxOutputs();
        if (maxOutputs > 1)
            globalOutputRange.setToStereo();
        if(!initPortAudio(sampleRate, bufferSize)){
            qCCritical(jtAudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
            audioDeviceIndex = paNoDevice;
        }
    } else {
        qCCritical(jtAudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        audioDeviceIndex = paNoDevice;
    }
}

QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const
{
    QList<int> buffersSize;
    buffersSize.append(256);
    return buffersSize;
}

QString PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    return "not implemented";
}

QString PortAudioDriver::getInputChannelName(const unsigned int index) const
{
    return "not implemented";
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
