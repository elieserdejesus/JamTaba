#include "PortAudioDriver.h"
#include "pa_asio.h"
#include "log/Logging.h"
#include "MainController.h"

namespace audio {

PortAudioDriver::PortAudioDriver(controller::MainController* mainController, QString audioInputDevice, QString audioOutputDevice, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize ) :
    AudioDriver(mainController),
    useSystemDefaultDevices(false)
{
    qCDebug(jtAudio) << QString("initializing portaudio (%1)...").arg(Pa_GetVersionText());
    auto error = Pa_Initialize();
    if (error != paNoError) {
        qCritical() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        return;
    }

    auto devicesNames = getDeviceNames();

    auto devicesFound = devicesNames.contains(audioInputDevice) && devicesNames.contains(audioOutputDevice);

    if (devicesFound) {
        audioInputDeviceIndex = devicesNames.indexOf(audioInputDevice);
        audioOutputDeviceIndex = UseSingleAudioIODevice ? audioInputDeviceIndex : devicesNames.indexOf(audioOutputDevice);
        globalInputRange = ChannelRange(firstInputIndex, (lastInputIndex - firstInputIndex) + 1);
        globalOutputRange = ChannelRange(firstOutputIndex, (lastOutputIndex - firstOutputIndex) + 1);
    }
    else {
        audioInputDeviceIndex = audioOutputDeviceIndex = paNoDevice; // forcing system default device
    }

    auto portAudioInitialized = initPortAudio(sampleRate, bufferSize);

    if (portAudioInitialized) {
        if (!devicesFound) {

            // if the previous device not found store the default system device as 'last used device'

            auto firstIn = globalInputRange.getFirstChannel();
            auto firstOut = globalOutputRange.getFirstChannel();
            auto lastIn = globalInputRange.getLastChannel();
            auto lastOut = globalOutputRange.getLastChannel();
            auto inputDevice = (audioInputDeviceIndex >= 0 && audioInputDeviceIndex < devicesNames.size()) ? devicesNames[audioInputDeviceIndex] : "";
            auto outputDevice = (audioOutputDeviceIndex >= 0 && audioOutputDeviceIndex < devicesNames.size()) ? devicesNames[audioOutputDeviceIndex] : "";

            mainController->storeIOSettings(
                        firstIn, lastIn,            // input channels
                        firstOut, lastOut,          // output channels
                        inputDevice, outputDevice   // I/O device names
            );
        }
    }
    else {
        audioInputDeviceIndex = audioOutputDeviceIndex = paNoDevice;
    }

}

void PortAudioDriver::configureHostSpecificInputParameters(PaStreamParameters& inputParams){

    //+++++++++++++++ ASIO SPECIFIC CODE FOR INPUT ++++++++++++++++++++++++++++++++
    qCDebug(jtAudio) << "using __WIN32 scpecific stream infos for inputs";
    PaAsioStreamInfo* asioInputInfo = new PaAsioStreamInfo;
    asioInputInfo->size = sizeof(PaAsioStreamInfo);
    asioInputInfo->hostApiType = paASIO;
    asioInputInfo->version = 1;
    asioInputInfo->flags = paAsioUseChannelSelectors;

    asioInputInfo->channelSelectors = new int[inputParams.channelCount];
    for (int c = 0; c < inputParams.channelCount; ++c) {
        asioInputInfo->channelSelectors[c] = globalInputRange.getFirstChannel() + c;//inputs are always sequential
    }
    inputParams.hostApiSpecificStreamInfo = asioInputInfo;
}

void PortAudioDriver::configureHostSpecificOutputParameters(PaStreamParameters& outputParams){
    //+++++++++++++++ ASIO SPECIFIC CODE FOR OUTPUT ++++++++++++++++++++++++++++++++
    qCDebug(jtAudio) << "using __WIN32 scpecific stream infos for outputs";
    PaAsioStreamInfo* asioOutputInfo = new PaAsioStreamInfo;
    asioOutputInfo->size = sizeof(PaAsioStreamInfo);
    asioOutputInfo->hostApiType = paASIO;
    asioOutputInfo->version = 1;
    asioOutputInfo->flags = paAsioUseChannelSelectors;
    asioOutputInfo->channelSelectors = new int[outputParams.channelCount];
    for (int c = 0; c < outputParams.channelCount; ++c) {
        asioOutputInfo->channelSelectors[c] = globalOutputRange.getFirstChannel() + c;//outputs are always sequential
    }
    outputParams.hostApiSpecificStreamInfo = asioOutputInfo;
}

void PortAudioDriver::releaseHostSpecificParameters(const PaStreamParameters &inputParameters, const PaStreamParameters &outputParameters){
    if(inputParameters.hostApiSpecificStreamInfo){
        PaAsioStreamInfo* asioInputStreamInfo = (PaAsioStreamInfo*)inputParameters.hostApiSpecificStreamInfo;
        delete asioInputStreamInfo->channelSelectors;
        delete asioInputStreamInfo;
    }
    if(outputParameters.hostApiSpecificStreamInfo){
        PaAsioStreamInfo* asioOutputStreamInfo = (PaAsioStreamInfo*)outputParameters.hostApiSpecificStreamInfo;
        delete asioOutputStreamInfo->channelSelectors;
        delete asioOutputStreamInfo;
    }
}

QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const{
    QList<int> bufferSizes;

    long maxBufferSize;
    long minBufferSize;
    long preferredBuffersize;
    long granularity;
    PaError result = PaAsio_GetAvailableBufferSizes(deviceIndex, &minBufferSize, &maxBufferSize, &preferredBuffersize, &granularity);
    if(result != paNoError){
        bufferSizes.append(256);
        return bufferSizes;//return 256 as the only possible value
    }
    //in windows ASIO when granularity is -1 the values are power of two
    for (long size = minBufferSize; size <= maxBufferSize; size *= 2) {
        bufferSizes.append(size);
    }
    return bufferSizes;
}

QString PortAudioDriver::getInputChannelName(const unsigned int index) const{
    /**
    From PaAsio_GetInputChannelName documentation: Retrieve a pointer to a string containing the name of the specified input channel. The string is valid until Pa_Terminate is called.
    The string will be no longer than 32 characters including the null terminator.
    */

    const char *channelName = nullptr;
    PaAsio_GetInputChannelName(audioInputDeviceIndex, index, &channelName);
    if(channelName){
        return QString(channelName);
    }
    return "Error";
}

QString PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    const char *channelName = nullptr;
    PaAsio_GetOutputChannelName(audioOutputDeviceIndex, index, &channelName);
    if(channelName){
        return QString(channelName);
    }
    return "Error";
}

bool PortAudioDriver::hasControlPanel() const{
    return true;
}

void PortAudioDriver::openControlPanel(void* mainWindowHandle){
    if(audioOutputDeviceIndex != paNoDevice){
        PaAsio_ShowControlPanel(audioOutputDeviceIndex, mainWindowHandle);
    }
}

void PortAudioDriver::preInitializePortAudioStream(PaStream *stream)
{
    Q_UNUSED(stream)
}

}//namespace
