#include "PortAudioDriver.h"
#include <QDebug>
#include <stdexcept>
#include <algorithm>
#include "portaudio.h"
#if _WIN32
    #include "pa_asio.h"
#endif

PortAudioDriver::PortAudioDriver()
{
    qDebug() << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qDebug() << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    paStream = inputBuffers = outputBuffers = NULL;

    inputDeviceIndex = Pa_GetDefaultInputDevice();
    outputDeviceIndex = Pa_GetDefaultOutputDevice();
    if(inputDeviceIndex != paNoDevice){
        inputChannels = Pa_GetDeviceInfo(inputDeviceIndex)->maxInputChannels;
        firstInputIndex = 0;
    }
    if(outputDeviceIndex != paNoDevice){
        outputChannels = 2;
        firstOutputIndex = 0;
    }
}

PortAudioDriver::~PortAudioDriver()
{
	qDebug() << "destrutor PortAudioDriver";
}

//this method just convert portaudio void* inputBuffer to a float[][] buffer, and do the same for outputs
void PortAudioDriver::translatePortAudioCallBack(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer){
    if(framesPerBuffer > BUFFERS_LENGHT){
        throw std::runtime_error(std::string("AUDIO ERROR: framesPerBuffer > BUFFERS_LENGHT"));

    }
    //prepare buffers and expose then do application process
    float* in = (float*)inputBuffer;
    for(unsigned int i=0; i < framesPerBuffer; i++){
        for (int c = 0; c < inputChannels; c++){
            inputBuffers[c][i] = *in++;
        }
    }

    for (int c = 0; c < outputChannels; c++){//zero output buffers
        memset(outputBuffers[c], 0, framesPerBuffer * sizeof(float));
    }

    //all application audio processing is computed here
    fireDriverCallback(inputBuffers, outputBuffers, framesPerBuffer);

    //convert application output buffers to portaudio format
    float* out = (float*)outputBuffer;
    for(unsigned int i=0; i < framesPerBuffer; i++){
        for (int c = 0; c < outputChannels; c++){
            *out++ = outputBuffers[c][i];
        }
    }
}

//friend function, receive the pointer to PortAudioDriver instance in userData param
int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* /*timeInfo*/,
    PaStreamCallbackFlags /*statusFlags*/, void *userData)
{
    //qDebug() << "portAudioCallBack framesPerBuffer: " << framesPerBuffer;
    PortAudioDriver* instance = (PortAudioDriver*)userData;
    instance->translatePortAudioCallBack(inputBuffer, outputBuffer, framesPerBuffer);
    return paContinue;
}

void PortAudioDriver::start(){
	stop();

    qDebug() << "recreating portaudio buffers...";
	recreateInputBuffers(BUFFERS_LENGHT);
	recreateOutputBuffers(BUFFERS_LENGHT);

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

	PaStreamParameters inputParams;
	inputParams.channelCount = inputChannels;
    inputParams.device = inputDeviceIndex;
	inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
	inputParams.hostApiSpecificStreamInfo = NULL;

	PaStreamParameters outputParams;
	outputParams.channelCount = outputChannels;
    outputParams.device = outputDeviceIndex;
	outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
	outputParams.hostApiSpecificStreamInfo = NULL;

    PaError error =  Pa_IsFormatSupported(&inputParams, &outputParams, sampleRate);
    if(error != paNoError){
        qDebug() << "unsuported format: " << Pa_GetErrorText(error);
    }

    if(inputDeviceIndex == outputDeviceIndex){
        qDebug() << "initializing " << Pa_GetDeviceInfo(inputDeviceIndex)->name << " inputs:" << inputParams.channelCount << " outputs:" << outputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
    }
    else{
        qDebug() << "initializing " << Pa_GetDeviceInfo(inputDeviceIndex)->name << " inputs:" << inputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
        qDebug() << "initializing " << Pa_GetDeviceInfo(outputDeviceIndex)->name << "outputs:" << outputParams.channelCount << " sampleRate:" << sampleRate << " bufferSize:" << bufferSize;
    }
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

PaDeviceIndex PortAudioDriver::getInputDeviceIndex() const
{
    return inputDeviceIndex;
}

void PortAudioDriver::setInputDeviceIndex(PaDeviceIndex index)
{
    stop();
    this->inputDeviceIndex = index;
}

PaDeviceIndex PortAudioDriver::getOutputDeviceIndex() const
{
    return outputDeviceIndex;
}

void PortAudioDriver::setOutputDeviceIndex(PaDeviceIndex index)
{
    stop();
    this->outputDeviceIndex = index;
}

const char *PortAudioDriver::getInputDeviceName(PaDeviceIndex index) const
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

//++++++++++++=

