#include "PortAudioDriver.h"
#include "pa_mac_core.h"
#include "../log/logging.h"

namespace Audio{


PortAudioDriver::PortAudioDriver(Controller::MainController* mainController,
                                 int deviceIndex, int firstInIndex,
                                 int lastInIndex, int firstOutIndex,
                                 int lastOutIndex, int sampleRate, int bufferSize )
    :AudioDriver(mainController){

    //initialize portaudio using default devices, mono input and try estereo output if possible
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qCCritical(jtAudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }

    this->audioDeviceIndex = Pa_GetDefaultOutputDevice();
    this->globalInputRange = ChannelRange(0, getMaxInputs());
    this->globalOutputRange = ChannelRange(0, 2);//2 channels for output

    int maxOutputs = getMaxOutputs();
    if(maxOutputs > 1){
        globalOutputRange.setToStereo();
    }
    initPortAudio(sampleRate, bufferSize);
}


QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const{
    QList<int> buffersSize;
    long maxBufferSize;
    long minBufferSize;
    PaError result = PaMacCore_GetBufferSizeRange(deviceIndex, &minBufferSize, &maxBufferSize);
    if(result != paNoError){
        buffersSize.append(256);
        return buffersSize;//return 256 as the only possible value
    }
    for (long size = minBufferSize; size <= maxBufferSize; size *= 2) {
        buffersSize.append(size);
    }
    return buffersSize;
}

const char *PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
    return PaMacCore_GetChannelName(audioDeviceIndex, index, false);
}

const char *PortAudioDriver::getInputChannelName(const unsigned int index) const
{
    return PaMacCore_GetChannelName(audioDeviceIndex, index, true);
}

void PortAudioDriver::configureHostSpecificInputParameters(PaStreamParameters& inputParameters){
    //qCDebug(portaudio) << "using MAC scpecific stream infos for inputs";
    //PaMacCoreStreamInfo streamInfo;
    //PaMacCore_SetupStreamInfo(&streamInfo, paMacCorePro);
    //inputParams.hostApiSpecificStreamInfo  = &streamInfo;
}

void PortAudioDriver::configureHostSpecificOutputParameters(PaStreamParameters& outputParameters){
    //qCDebug(portaudio) << "using MAC scpecific stream infos for output";
    //PaMacCoreStreamInfo outStreamInfo;
    //PaMacCore_SetupStreamInfo(&outStreamInfo, paMacCorePro);
    //outputParams.hostApiSpecificStreamInfo = &outStreamInfo;
}

void PortAudioDriver::releaseHostSpecificParameters(const PaStreamParameters &inputParameters, const PaStreamParameters &outputParameters){

}

bool PortAudioDriver::hasControlPanel() const{
    return false;
}

}//namespace
