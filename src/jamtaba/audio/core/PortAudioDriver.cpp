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

PortAudioDriver::PortAudioDriver(){
    //initialize portaudio using default devices, mono input and try estereo output if possible
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qDebug() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    int outputs = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice())->maxOutputChannels;
    initPortAudio(Pa_GetDefaultInputDevice(), Pa_GetDefaultOutputDevice(), 0, 0, 0, outputs > 1 ? 1 : 0 , 44100, paFramesPerBufferUnspecified);
}

PortAudioDriver::PortAudioDriver(int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize )
{
    //initialize port audio using custom devices and I/O
    initPortAudio(inputDeviceIndex, outputDeviceIndex, firstInputIndex, lastInputIndex, firstOutputIndex, lastOutputIndex, sampleRate, bufferSize);
}

void PortAudioDriver::initPortAudio(int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize)
{
    //qDebug() << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qDebug() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    paStream = inputBuffer = outputBuffer = NULL;

    //set input device
    if(inputDeviceIndex < 0 || inputDeviceIndex >= Pa_GetDeviceCount()){
        inputDeviceIndex = Pa_GetDefaultInputDevice();
    }

    //set output device
    if(outputDeviceIndex < 0 || outputDeviceIndex >= Pa_GetDeviceCount()){
        outputDeviceIndex = Pa_GetDefaultOutputDevice();
    }

    //set input channels
    if(inputDeviceIndex != paNoDevice){
        int lastInputsCount = lastInputIndex - firstInputIndex + 1;
        int maxInputs = Pa_GetDeviceInfo(inputDeviceIndex)->maxInputChannels;
        if(lastInputsCount > maxInputs || firstInputIndex >= maxInputs ){
            firstInputIndex = Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowInputLatency;
            inputChannels = (firstInputIndex + lastInputsCount < maxInputs) ? lastInputsCount : 1 ;//force mono input
        }
        else{
            inputChannels = lastInputsCount;
        }
    }

    //set output channels
    if(outputDeviceIndex != paNoDevice){
        this->firstOutputIndex = firstOutputIndex;
        int lastOutputsCount = lastOutputIndex - firstOutputIndex + 1;
        int maxOutputs = Pa_GetDeviceInfo(outputDeviceIndex)->maxOutputChannels;
        if(lastOutputsCount > maxOutputs || firstOutputIndex >= maxOutputs ){
            firstOutputIndex = Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowInputLatency;
            outputChannels = (firstOutputIndex + lastOutputsCount < maxOutputs) ? lastOutputsCount : 2 ;//force mono input
        }
        else{
            outputChannels = lastOutputsCount;
        }
    }

    //set sample rate
    this->sampleRate = sampleRate >= 44100 ? sampleRate : 44100;

    this->bufferSize = bufferSize > 0 ? bufferSize : paFramesPerBufferUnspecified;
}

PortAudioDriver::~PortAudioDriver()
{
    //qDebug() << "destrutor PortAudioDriver";
}



//this method just convert portaudio void* inputBuffer to a float[][] buffer, and do the same for outputs
void PortAudioDriver::translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer){
    if(framesPerBuffer > MAX_BUFFERS_LENGHT){
        qFatal("AUDIO ERROR: framesPerBuffer > BUFFERS_LENGHT");
    }

    //prepare buffers and expose then do application process
    inputBuffer->setFrameLenght(framesPerBuffer);
    outputBuffer->setFrameLenght(framesPerBuffer);

    float* inputs = (float*)in;
    for(unsigned int i=0; i < framesPerBuffer; i++){
        for (int c = 0; c < maxInputChannels; c++){
            inputBuffer->set(c, i, *inputs++);
        }
    }

    outputBuffer->zero();

    //all application audio processing is computed here
    fireDriverCallback(*inputBuffer, *outputBuffer);

    //convert application output buffers to portaudio format
    float* outputs = (float*)out;
    for(unsigned int i=0; i < framesPerBuffer; i++){
        for (int c = 0; c < maxOutputChannels; c++){
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

    //qDebug() << "recreating portaudio buffers...";

    int maxOutputs = Pa_GetDeviceInfo(outputDeviceIndex)->maxOutputChannels;
    int maxInputs = Pa_GetDeviceInfo(inputDeviceIndex)->maxInputChannels;

    recreateBuffers(MAX_BUFFERS_LENGHT, maxInputs, maxOutputs);

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = maxInputs;//*/ inputChannels;
    inputParams.device = inputDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    PaStreamParameters outputParams;
    outputParams.channelCount = maxOutputs;// */outputChannels;
    outputParams.device = outputDeviceIndex;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    PaError error =  Pa_IsFormatSupported(&inputParams, &outputParams, sampleRate);
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
    error = Pa_OpenStream(&paStream, &inputParams, &outputParams, sampleRate, framesPerBuffer, paNoFlag, portaudioCallBack, (void*)this);//I'm passing this to portaudio, so I can run methods inside the callback function
    if (error != paNoError){
        throw std::runtime_error(std::string(Pa_GetErrorText(error)));
    }
    if(paStream != NULL){
        error = Pa_StartStream(paStream);
        if (error != paNoError){
            throw std::runtime_error(Pa_GetErrorText(error));
        }
    }
    fireDriverStarted();

}

void PortAudioDriver::stop(){
    if (paStream != NULL){
        if (!Pa_IsStreamStopped(paStream)){
            PaError error = Pa_CloseStream(paStream);
            if(error != paNoError){
                qDebug() << Pa_GetErrorText(error);
                throw std::runtime_error(std::string(Pa_GetErrorText(error)));
            }
            fireDriverStopped();
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

