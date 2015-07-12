#include "PortAudioDriver.h"
#include <QDebug>
#include <stdexcept>
#include <algorithm>
#include "portaudio.h"
#include "SamplesBuffer.h"
#include "../persistence/ConfigStore.h"

#include <QThread>

#if _WIN32
    #include "pa_asio.h"
#endif

namespace Audio{

PortAudioDriver::PortAudioDriver(AudioDriverListener *audioDriverListener)
    :AudioDriver(audioDriverListener)
{
    //initialize portaudio using default devices, mono input and try estereo output if possible
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qDebug() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    this->globalInputRange = ChannelRange(0, 1);//one channel for input
    this->globalOutputRange = ChannelRange(0, 2);//2 channels for output
    this->inputDeviceIndex = Pa_GetDefaultInputDevice();
    this->outputDeviceIndex = Pa_GetDefaultOutputDevice();
    int maxOutputs = getMaxOutputs();
    if(maxOutputs > 1){
        globalOutputRange.setToStereo();
    }
    initPortAudio(44100, paFramesPerBufferUnspecified);
}

PortAudioDriver::PortAudioDriver(AudioDriverListener* audioDriverListener, int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize )
    :AudioDriver(audioDriverListener)
{
    this->globalInputRange = ChannelRange(firstInputIndex, (lastInputIndex - firstInputIndex) + 1);
    this->globalOutputRange = ChannelRange(firstOutputIndex, (lastOutputIndex - firstOutputIndex) + 1);
    this->inputDeviceIndex = inputDeviceIndex;
    this->outputDeviceIndex = outputDeviceIndex;
    initPortAudio(sampleRate, bufferSize);
}

void PortAudioDriver::initPortAudio(int sampleRate, int bufferSize)
{
    //qDebug() << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qDebug() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
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
        if(inputsCount > maxInputs || globalInputRange.getFirstChannel() >= maxInputs ){
            //force input to mono
            globalInputRange = ChannelRange( Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowInputLatency, 1);
        }
    }

    //check if outputs are valid
    if(outputDeviceIndex != paNoDevice){
        int outputsCount = globalOutputRange.getChannels();
        int maxOutputs = getMaxOutputs();
        if(outputsCount > maxOutputs || globalOutputRange.getFirstChannel() >= maxOutputs ){
            globalOutputRange = ChannelRange(Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency, 1);
        }
    }

    //set sample rate
    this->sampleRate = (sampleRate >= 44100 && sampleRate <= 192000) ? sampleRate : 44100;

    this->bufferSize = (bufferSize >= 64 && bufferSize <= 4096) ? bufferSize : paFramesPerBufferUnspecified;
}

PortAudioDriver::~PortAudioDriver()
{
    //qDebug() << "destrutor PortAudioDriver";
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
    if(audioDriverListener){
        audioDriverListener->process(*inputBuffer, *outputBuffer);
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
    stop();

    recreateBuffers();//adjust the input and output buffers channels

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = globalInputRange.getChannels();// maxInputChannels;//*/ inputChannels;
    inputParams.device = inputDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

//+++++++++++++++ ASIO SPECIFIC CODE FOR INPUT ++++++++++++++++++++++++++++++++
#ifdef _WIN32
    PaAsioStreamInfo asioInputInfo;
    asioInputInfo.size = sizeof(PaAsioStreamInfo);
    asioInputInfo.hostApiType = paASIO;
    asioInputInfo.version = 1;
    asioInputInfo.flags = paAsioUseChannelSelectors;
    int inputChannelSelectors[inputParams.channelCount];
    for (int c = 0; c < inputParams.channelCount; ++c) {
        inputChannelSelectors[c] = globalInputRange.getFirstChannel() + c;//inputs are always sequential
    }
    asioInputInfo.channelSelectors = inputChannelSelectors;
    inputParams.hostApiSpecificStreamInfo = &asioInputInfo;
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    PaStreamParameters outputParams;
    outputParams.channelCount = globalOutputRange.getChannels();// */outputChannels;
    outputParams.device = outputDeviceIndex;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

//+++++++++++++++ ASIO SPECIFIC CODE FOR OUTPUT ++++++++++++++++++++++++++++++++
#ifdef _WIN32
    PaAsioStreamInfo asioOutputInfo;
    asioOutputInfo.size = sizeof(PaAsioStreamInfo);
    asioOutputInfo.hostApiType = paASIO;
    asioOutputInfo.version = 1;
    asioOutputInfo.flags = paAsioUseChannelSelectors;
    int outputChannelSelectors[outputParams.channelCount];
    for (int c = 0; c < outputParams.channelCount; ++c) {
        outputChannelSelectors[c] = globalOutputRange.getFirstChannel() + c;//outputs are always sequential
    }
    asioOutputInfo.channelSelectors = outputChannelSelectors;
    outputParams.hostApiSpecificStreamInfo = &asioOutputInfo;
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    PaError error =  Pa_IsFormatSupported( !globalInputRange.isEmpty() ? (&inputParams) : NULL, &outputParams, sampleRate);
    if(error != paNoError){
        qDebug() << "unsuported format: " << Pa_GetErrorText(error);
        throw std::runtime_error(std::string(Pa_GetErrorText(error) ));
    }

//    if(inputDeviceIndex == outputDeviceIndex){
//        qDebug() << "initializing " << Pa_GetDeviceInfo(inputDeviceIndex)->name << " inputs:" << inputParams.channelCount << " outputs:" << outputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
//    }
//    else{
//        qDebug() << "initializing " << Pa_GetDeviceInfo(inputDeviceIndex)->name << " inputs:" << inputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
//        qDebug() << "initializing " << Pa_GetDeviceInfo(outputDeviceIndex)->name << "outputs:" << outputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
//    }
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
    emit started();
}

void PortAudioDriver::stop(){
    //QMutexLocker(&mutex);
    if (paStream != NULL){
        if (!Pa_IsStreamStopped(paStream)){
            PaError error = Pa_CloseStream(paStream);
            if(error != paNoError){
                qDebug() << Pa_GetErrorText(error);
                throw std::runtime_error(std::string(Pa_GetErrorText(error)));
            }
            emit stopped(); //fireDriverStopped();
        }
    }
}

void PortAudioDriver::release(){
    stop();
    Pa_Terminate();
}

int PortAudioDriver::getMaxInputs() const{
    if(inputDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(inputDeviceIndex)->maxInputChannels;
    }
    return 0;
}
int PortAudioDriver::getMaxOutputs() const{
    if(outputDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(outputDeviceIndex)->maxOutputChannels;
    }
    return 0;
}

const char *PortAudioDriver::getInputChannelName(const unsigned int index) const
{
#if _WIN32
    const char *channelName = new char[30];
    PaAsio_GetInputChannelName(inputDeviceIndex, index, &channelName);
    return channelName;
#endif /* _WIN32*/
    return "error";
}

const char *PortAudioDriver::getOutputChannelName(const unsigned int index) const
{
#if _WIN32
    const char *channelName = new char[30];
    PaAsio_GetOutputChannelName(inputDeviceIndex, index, &channelName);
    return channelName;
#endif /* _WIN32*/
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

