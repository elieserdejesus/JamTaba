#include "PortAudioDriver.h"
#include "pa_asio.h"
#include "log/Logging.h"

namespace Audio{

PortAudioDriver::PortAudioDriver(Controller::MainController* mainController, int deviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize )
    :AudioDriver(mainController)
{
    globalInputRange = ChannelRange(firstInputIndex, (lastInputIndex - firstInputIndex) + 1);
    globalOutputRange = ChannelRange(firstOutputIndex, (lastOutputIndex - firstOutputIndex) + 1);
    audioDeviceIndex = deviceIndex;
    if(!initPortAudio(sampleRate, bufferSize)){
        audioDeviceIndex = paNoDevice;
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

const char *PortAudioDriver::getInputChannelName(const unsigned int index) const{
    const char *channelName = new char[30];
    PaAsio_GetInputChannelName(audioDeviceIndex, index, &channelName);
    return channelName;
}

const char *PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    const char *channelName = new char[30];
    PaAsio_GetOutputChannelName(audioDeviceIndex, index, &channelName);
    return channelName;
}

bool PortAudioDriver::hasControlPanel() const{
    return true;
}

void PortAudioDriver::openControlPanel(void* mainWindowHandle){
    if(audioDeviceIndex != paNoDevice){
        PaAsio_ShowControlPanel(audioDeviceIndex, mainWindowHandle);
    }
}

}//namespace
