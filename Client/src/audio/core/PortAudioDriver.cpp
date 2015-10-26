#include "PortAudioDriver.h"
#include <QDebug>
#include <stdexcept>
#include <algorithm>
#include "portaudio.h"
#include "SamplesBuffer.h"
#include "../persistence/Settings.h"
#include "../MainController.h"
#include <QtGlobal>
//#include <QThread>
#ifdef Q_OS_WIN
    #include "pa_asio.h"
#else
    #include "pa_mac_core.h"
#endif

Q_LOGGING_CATEGORY(portaudio, "portaudio")


namespace Audio{

#ifdef Q_OS_MACX
    PortAudioDriver::PortAudioDriver(Controller::MainController* mainController, int sampleRate, int  bufferSize)
        :AudioDriver(mainController){

            //initialize portaudio using default devices, mono input and try estereo output if possible
            PaError error = Pa_Initialize();
            if (error != paNoError){
                qCCritical(portaudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
                throw std::runtime_error(Pa_GetErrorText(error));
            }

            this->inputDeviceIndex = Pa_GetDefaultInputDevice();
            this->outputDeviceIndex = Pa_GetDefaultOutputDevice();
            this->globalInputRange = ChannelRange(0, getMaxInputs());
            this->globalOutputRange = ChannelRange(0, 2);//2 channels for output

            int maxOutputs = getMaxOutputs();
            if(maxOutputs > 1){
                globalOutputRange.setToStereo();
            }
            initPortAudio(sampleRate, bufferSize);
    }
#endif

#ifdef Q_OS_WIN
PortAudioDriver::PortAudioDriver(Controller::MainController* mainController, int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize )
    :AudioDriver(mainController)
{
    this->globalInputRange = ChannelRange(firstInputIndex, (lastInputIndex - firstInputIndex) + 1);
    this->globalOutputRange = ChannelRange(firstOutputIndex, (lastOutputIndex - firstOutputIndex) + 1);
    this->inputDeviceIndex = inputDeviceIndex;// (inputDeviceIndex >= 0 && inputDeviceIndex < getDevicesCount()) ? inputDeviceIndex : paNoDevice;
    this->outputDeviceIndex = outputDeviceIndex;// (outputDeviceIndex >= 0 && outputDeviceIndex < getDevicesCount()) ? outputDeviceIndex : paNoDevice;
    initPortAudio(sampleRate, bufferSize);
}
#endif

bool PortAudioDriver::canBeStarted() const{
    return deviceIndexedAreValid();
}

bool PortAudioDriver::deviceIndexedAreValid() const{
    return outputDeviceIndex != paNoDevice && inputDeviceIndex != paNoDevice;
}

void PortAudioDriver::initPortAudio(int sampleRate, int bufferSize)
{
    qCInfo(portaudio) << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qCCritical(portaudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    paStream = nullptr;// inputBuffer = outputBuffer = NULL;

    //check for invalid input device index
    if(inputDeviceIndex < 0 || inputDeviceIndex >= Pa_GetDeviceCount()){
        inputDeviceIndex = Pa_GetDefaultInputDevice();
    }

    //check for invalid output device index
    if(outputDeviceIndex < 0 || outputDeviceIndex >= Pa_GetDeviceCount()){
        outputDeviceIndex = Pa_GetDefaultOutputDevice();
    }

    //check if inputs are valid for selected device
    if(inputDeviceIndex != paNoDevice){
        int inputsCount = globalInputRange.getChannels();
        int maxInputs = getMaxInputs();
        if(inputsCount > maxInputs || globalInputRange.getFirstChannel() >= maxInputs || inputsCount <= 0 ){
            //const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(inputDeviceIndex);
            globalInputRange = ChannelRange( 0, std::min(maxInputs, 1));
        }
    }

    //check if outputs are valid
    if(outputDeviceIndex != paNoDevice){
        int outputsCount = globalOutputRange.getChannels();
        int maxOutputs = getMaxOutputs();
        if(outputsCount > maxOutputs || globalOutputRange.getFirstChannel() >= maxOutputs || outputsCount <= 0){
            const PaDeviceInfo* info = Pa_GetDeviceInfo(outputDeviceIndex);
            globalOutputRange = ChannelRange(info->defaultLowOutputLatency, std::min(2, info->maxOutputChannels));
        }
    }

    //set sample rate
    this->sampleRate = (sampleRate >= 44100 && sampleRate <= 192000) ? sampleRate : 44100;
    if(deviceIndexedAreValid()){
        QList<int> validSampleRates = getValidSampleRates(outputDeviceIndex);
        if(this->sampleRate > validSampleRates.last()){
            this->sampleRate = validSampleRates.last();//use the max supported sample rate
        }
    }

    this->bufferSize = bufferSize;
    if(deviceIndexedAreValid()){
        QList<int> validBufferSizes = getValidBufferSizes(outputDeviceIndex);
        if(this->bufferSize < validBufferSizes.first()){
            this->bufferSize = validBufferSizes.first();//use the minimum supported buffer size
        }
        if(this->bufferSize > validBufferSizes.last()){
            this->bufferSize = validBufferSizes.last();//use the max supported buffer size
        }
    }
}

PortAudioDriver::~PortAudioDriver()
{
    qCDebug(portaudio) << "destructor PortAudioDriver";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//this method just convert portaudio void* inputBuffer to a float[][] buffer, and do the same for outputs
void PortAudioDriver::translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer){
    if(!inputBuffer || !outputBuffer){
        return;
    }
    //prepare buffers and expose then to application process
    inputBuffer->setFrameLenght(framesPerBuffer);
    outputBuffer->setFrameLenght(framesPerBuffer);
    if(!globalInputRange.isEmpty()){
        float* inputs = (float*)in;
        int inputChannels = globalInputRange.getChannels();
        for(unsigned int i=0; i < framesPerBuffer; i++){
            for (int c = 0; c < inputChannels; c++){
                inputBuffer->set(c, i, *inputs++);
            }
        }
    }
    else{
        inputBuffer->zero();
    }

    outputBuffer->zero();


    //all application audio processing is computed here
    if(mainController){
        mainController->process(*inputBuffer, *outputBuffer, sampleRate);
    }

    //convert application output buffers to portaudio format
    float* outputs = (float*)out;
    int outputChannels = globalOutputRange.getChannels();
    for(unsigned int i=0; i < framesPerBuffer; i++){
        for (int c = 0; c < outputChannels; c++){
            *outputs++ = outputBuffer->get(c, i);
        }
    }
}

//friend function, receive the pointer to PortAudioDriver instance in userData param
int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
                      unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* /*timeInfo*/,
                      PaStreamCallbackFlags /*statusFlags*/, void *userData)
{
    //qDebug() << "portAudioCallBack  Thread ID: " << QThread::currentThreadId();
    PortAudioDriver* instance = static_cast<PortAudioDriver*>(userData);
    instance->translatePortAudioCallBack(inputBuffer, outputBuffer, framesPerBuffer);
    return paContinue;
}


void PortAudioDriver::start(){

    if(inputDeviceIndex == paNoDevice || outputDeviceIndex == paNoDevice){
        return;
    }

    stop();

    qCInfo(portaudio) << "Starting portaudio driver using" << getInputDeviceName(inputDeviceIndex);

    recreateBuffers();//adjust the input and output buffers channels

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = globalInputRange.getChannels();// maxInputChannels;//*/ inputChannels;
    inputParams.device = inputDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);// Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

//+++++++++++++++ ASIO SPECIFIC CODE FOR INPUT ++++++++++++++++++++++++++++++++
#ifdef Q_OS_WIN
    qCDebug(portaudio) << "using __WIN32 scpecific stream infos for inputs";
    PaAsioStreamInfo asioInputInfo;
    asioInputInfo.size = sizeof(PaAsioStreamInfo);
    asioInputInfo.hostApiType = paASIO;
    asioInputInfo.version = 1;
    asioInputInfo.flags = paAsioUseChannelSelectors;
	//const int size = ;
    //int inputChannelSelectors[size];
    std::vector<int> inputChannelSelectors(inputParams.channelCount);
    for (int c = 0; c < inputParams.channelCount; ++c) {
        inputChannelSelectors[c] = globalInputRange.getFirstChannel() + c;//inputs are always sequential
    }
    asioInputInfo.channelSelectors = inputChannelSelectors.data();
    inputParams.hostApiSpecificStreamInfo = &asioInputInfo;
#endif

//#ifdef Q_OS_MACX
//    qCDebug(portaudio) << "using MAC scpecific stream infos for inputs";
//    PaMacCoreStreamInfo streamInfo;
//    PaMacCore_SetupStreamInfo(&streamInfo, paMacCorePro);
//    //inputParams.hostApiSpecificStreamInfo  = &streamInfo;
//#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    PaStreamParameters outputParams;
    outputParams.channelCount = globalOutputRange.getChannels();// */outputChannels;
    outputParams.device = outputDeviceIndex;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);//  Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

//+++++++++++++++ ASIO SPECIFIC CODE FOR OUTPUT ++++++++++++++++++++++++++++++++
#ifdef Q_OS_WIN
    qCDebug(portaudio) << "using __WIN32 scpecific stream infos for outputs";
    PaAsioStreamInfo asioOutputInfo;
    asioOutputInfo.size = sizeof(PaAsioStreamInfo);
    asioOutputInfo.hostApiType = paASIO;
    asioOutputInfo.version = 1;
    asioOutputInfo.flags = paAsioUseChannelSelectors;
    std::vector<int> outputChannelSelectors(outputParams.channelCount);
    for (int c = 0; c < outputParams.channelCount; ++c) {
        outputChannelSelectors[c] = globalOutputRange.getFirstChannel() + c;//outputs are always sequential
    }
    asioOutputInfo.channelSelectors = outputChannelSelectors.data();
    outputParams.hostApiSpecificStreamInfo = &asioOutputInfo;
#endif

//#ifdef Q_OS_MACX
//    qCDebug(portaudio) << "using MAC scpecific stream infos for output";
//    PaMacCoreStreamInfo outStreamInfo;
//    PaMacCore_SetupStreamInfo(&outStreamInfo, paMacCorePro);
//    //outputParams.hostApiSpecificStreamInfo = &outStreamInfo;
//#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    PaError error =  Pa_IsFormatSupported( !globalInputRange.isEmpty() ? (&inputParams) : NULL, &outputParams, sampleRate);
    if(error != paNoError){
        qCCritical(portaudio) << "unsuported format: " << Pa_GetErrorText(error) << "sampleRate: " << sampleRate ;
        throw std::runtime_error(std::string(Pa_GetErrorText(error) ));
    }

    paStream = NULL;
    error = Pa_OpenStream(&paStream,
                          (!globalInputRange.isEmpty()) ? (&inputParams) : NULL,
                          &outputParams,
                          sampleRate, framesPerBuffer,
                          paNoFlag, portaudioCallBack, (void*)this);//I'm passing this to portaudio, so I can run methods inside the callback function
    if (error != paNoError){
        throw std::runtime_error(std::string(Pa_GetErrorText(error)));
    }
    if(paStream != NULL){
        error = Pa_StartStream(paStream);
        if (error != paNoError){
            throw std::runtime_error(Pa_GetErrorText(error));
        }
    }
    qCInfo(portaudio) << "portaudio driver started ok!";
    emit started();

}

QList<int> PortAudioDriver::getValidSampleRates(int deviceIndex) const{
    PaStreamParameters outputParams;
    outputParams.channelCount = 1;
    outputParams.device = deviceIndex;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;
    QList<int> validSRs;
    validSRs.append(44100);
    validSRs.append(48000);
    validSRs.append(96000);
    validSRs.append(192000);
    for (int t = validSRs.size()-1; t >= 0; --t) {
        int sampleRate = validSRs.at(t);
        PaError error =  Pa_IsFormatSupported(  NULL, &outputParams, sampleRate);
        if(error != paNoError){
            validSRs.removeAt(t);
        }
        else{
            break;//test bigger sample rates first, and when the first big sample rate pass the test assume all other small sample rates are ok
        }
    }
    return validSRs;
}

QList<int> PortAudioDriver::getValidBufferSizes(int deviceIndex) const{

    QList<int> completeList;


    long maxBufferSize;
    long minBufferSize;

    #ifdef Q_OS_WIN
        long preferredBuffersize;//note used
        long granularity;//note used
        PaError result = PaAsio_GetAvailableBufferSizes(deviceIndex, &minBufferSize, &maxBufferSize, &preferredBuffersize, &granularity);
    #else//mac
        PaError result = PaMacCore_GetBufferSizeRange(deviceIndex, &minBufferSize, &maxBufferSize);
    #endif
    if(result != paNoError){
        completeList.append(256);
        return completeList;//return 256 as the only possible value
    }
    //in windows ASIO when granularity is -1 the values are power of two
    for (long size = minBufferSize; size <= maxBufferSize; size *= 2) {
        completeList.append(size);
    }

    return completeList;
}

void PortAudioDriver::stop(){
    //QMutexLocker(&mutex);
    if (paStream != NULL){
        if (!Pa_IsStreamStopped(paStream)){
            qCDebug(portaudio) << "closing portaudio stream";
            PaError error = Pa_CloseStream(paStream);
            if(error != paNoError){
                qCCritical(portaudio) << "error closing portaudio stream: " << Pa_GetErrorText(error);
                throw std::runtime_error(std::string(Pa_GetErrorText(error)));
            }
            emit stopped(); //fireDriverStopped();
        }
    }
}

void PortAudioDriver::release(){
    qCDebug(portaudio) << "releasing portaudio resources...";
    stop();
    Pa_Terminate();
    qCDebug(portaudio) << "portaudio terminated!";
}

int PortAudioDriver::getMaxInputs() const{

    if(inputDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(inputDeviceIndex)->maxInputChannels;
    }
    #ifdef Q_OS_WIN
        return 0;
    #endif
    //mac
    return Pa_GetDeviceInfo(Pa_GetDefaultInputDevice())->maxInputChannels;
}
int PortAudioDriver::getMaxOutputs() const{
    if(outputDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(outputDeviceIndex)->maxOutputChannels;
    }
#ifdef Q_OS_WIN
    return 0;
#endif
    return Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->maxOutputChannels;
}

const char *PortAudioDriver::getInputChannelName(const unsigned int index) const
{

#ifdef Q_OS_WIN
    const char *channelName = new char[30];
    PaAsio_GetInputChannelName(inputDeviceIndex, index, &channelName);
    return channelName;
#else
    return PaMacCore_GetChannelName(this->inputDeviceIndex, index, true);
#endif
    return "error";
}

const char *PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
#ifdef Q_OS_WIN
    const char *channelName = new char[30];
    PaAsio_GetOutputChannelName(inputDeviceIndex, index, &channelName);
    return channelName;
#else
    return PaMacCore_GetChannelName(outputDeviceIndex, index, false);
#endif
    return "error";
}

int PortAudioDriver::getInputDeviceIndex() const
{
    return inputDeviceIndex;
}

void PortAudioDriver::setInputDeviceIndex(PaDeviceIndex index)
{
    stop();
    this->inputDeviceIndex = index;
}

int PortAudioDriver::getOutputDeviceIndex() const
{
    return outputDeviceIndex;
}

void PortAudioDriver::setOutputDeviceIndex(int index)
{
    stop();
    this->outputDeviceIndex = index;
}

const char *PortAudioDriver::getInputDeviceName(int index) const
{
    return Pa_GetDeviceInfo(index)->name;
}

const char *PortAudioDriver::getOutputDeviceName(PaDeviceIndex index) const
{
    return Pa_GetDeviceInfo(index)->name;
}

int PortAudioDriver::getDevicesCount() const
{
    return Pa_GetDeviceCount();
}

}
//++++++++++++=

