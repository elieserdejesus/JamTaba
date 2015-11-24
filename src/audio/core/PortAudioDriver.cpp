#include "PortAudioDriver.h"
#include <QDebug>
#include <stdexcept>
#include <algorithm>
#include "portaudio.h"
#include "SamplesBuffer.h"
#include "../persistence/Settings.h"
#include "../MainController.h"
#include "../log/logging.h"
#include <QtGlobal>

/*
 * This file contain the platform independent PortAudio code. The platform specific
 * code are in WindowsPortAudioDriver.cpp, MacPortAudioDriver.cpp and in future LinuxPortAudioDriver.cpp.
*/

namespace Audio{

bool PortAudioDriver::canBeStarted() const{
    return audioDeviceIndex != paNoDevice;
}

void PortAudioDriver::initPortAudio(int sampleRate, int bufferSize)
{
    qCInfo(jtAudio) << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qCCritical(jtAudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        throw std::runtime_error(Pa_GetErrorText(error));
    }
    paStream = nullptr;// inputBuffer = outputBuffer = NULL;

    //check for invalid audio device index
    if(audioDeviceIndex < 0 || audioDeviceIndex >= Pa_GetDeviceCount()){
        audioDeviceIndex = Pa_GetDefaultInputDevice();
        if(audioDeviceIndex == paNoDevice){
            audioDeviceIndex = Pa_GetDefaultOutputDevice();
        }
    }


    //check if inputs are valid for selected device
    if(audioDeviceIndex != paNoDevice){
        int inputsCount = globalInputRange.getChannels();
        int maxInputs = getMaxInputs();
        if(inputsCount > maxInputs || globalInputRange.getFirstChannel() >= maxInputs || inputsCount <= 0 ){
            //const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(inputDeviceIndex);
            globalInputRange = ChannelRange( 0, std::min(maxInputs, 1));
        }
    }

    //check if outputs are valid
    if(audioDeviceIndex != paNoDevice){
        int outputsCount = globalOutputRange.getChannels();
        int maxOutputs = getMaxOutputs();
        if(outputsCount > maxOutputs || globalOutputRange.getFirstChannel() >= maxOutputs || outputsCount <= 0){
            const PaDeviceInfo* info = Pa_GetDeviceInfo(audioDeviceIndex);
            globalOutputRange = ChannelRange(info->defaultLowOutputLatency, std::min(2, info->maxOutputChannels));
        }
    }

    //set sample rate
    this->sampleRate = (sampleRate >= 44100 && sampleRate <= 192000) ? sampleRate : 44100;
    if(audioDeviceIndex != paNoDevice){//avoid query sample rates in a invalid device
        QList<int> validSampleRates = getValidSampleRates(audioDeviceIndex);
        if(this->sampleRate > validSampleRates.last()){
            this->sampleRate = validSampleRates.last();//use the max supported sample rate
        }
    }

    this->bufferSize = bufferSize;
    if(audioDeviceIndex != paNoDevice){
        QList<int> validBufferSizes = getValidBufferSizes(audioDeviceIndex);
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
    qCDebug(jtAudio) << "destructor PortAudioDriver";
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

    if(audioDeviceIndex == paNoDevice){
        return;
    }

    stop();

    qCInfo(jtAudio) << "Starting portaudio driver using" << getAudioDeviceName(audioDeviceIndex) << " as device.";

    recreateBuffers();//adjust the input and output buffers channels

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    qCInfo(jtAudio) << "Starting portaudio driver using" << framesPerBuffer << " as buffer size.";
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = globalInputRange.getChannels();// maxInputChannels;//*/ inputChannels;
    inputParams.device = audioDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);// Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    configureHostSpecificInputParameters(inputParams);//this can be different in different operational systems

    //+++++++++ OUTPUT
    PaStreamParameters outputParams;
    outputParams.channelCount = globalOutputRange.getChannels();// */outputChannels;
    outputParams.device = audioDeviceIndex;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);//  Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    configureHostSpecificOutputParameters(outputParams);

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    bool willUseInputParams = !globalInputRange.isEmpty();
    if(willUseInputParams){
        qCInfo(jtAudio) << "Trying initialize portaudio using inputParams and samplerate=" << sampleRate;
    }
    else{
        qCInfo(jtAudio) << "Trying initialize portaudio WITHOUT inputParams because globalInputRange is empty!";
        qCInfo(jtAudio) << "Detected inputs for " << getAudioDeviceName(audioDeviceIndex) << ":" << getMaxInputs();
    }

    PaError error =  Pa_IsFormatSupported( !globalInputRange.isEmpty() ? (&inputParams) : NULL, &outputParams, sampleRate);
    if(error != paNoError){
        qCCritical(jtAudio) << "unsuported format: " << Pa_GetErrorText(error) << "sampleRate: " << sampleRate ;
        this->audioDeviceIndex = paNoDevice;
        const char* errorMsg = Pa_GetErrorText(error);
        qCCritical(jtAudio) << "Error message: " << QString::fromUtf8(errorMsg);
        releaseHostSpecificParameters(inputParams, outputParams);
        throw std::runtime_error(std::string(errorMsg));
    }

    paStream = NULL;
    error = Pa_OpenStream(&paStream,
                          (!globalInputRange.isEmpty()) ? (&inputParams) : NULL,
                          &outputParams,
                          sampleRate, framesPerBuffer,
                          paNoFlag, portaudioCallBack, (void*)this);//I'm passing this to portaudio, so I can run methods inside the callback function
    if (error != paNoError){
        releaseHostSpecificParameters(inputParams, outputParams);
        throw std::runtime_error(std::string(Pa_GetErrorText(error)));
    }
    if(paStream != NULL){
        error = Pa_StartStream(paStream);
        if (error != paNoError){
            releaseHostSpecificParameters(inputParams, outputParams);
            throw std::runtime_error(Pa_GetErrorText(error));
        }
    }
    qCInfo(jtAudio) << "portaudio driver started ok!";
    emit started();

    releaseHostSpecificParameters(inputParams, outputParams);
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

void PortAudioDriver::stop(){
    //QMutexLocker(&mutex);
    if (paStream != NULL){
        if (!Pa_IsStreamStopped(paStream)){
            qCDebug(jtAudio) << "closing portaudio stream";
            PaError error = Pa_CloseStream(paStream);
            if(error != paNoError){
                qCCritical(jtAudio) << "error closing portaudio stream: " << Pa_GetErrorText(error);
                throw std::runtime_error(std::string(Pa_GetErrorText(error)));
            }
            emit stopped(); //fireDriverStopped();
        }
    }
}

void PortAudioDriver::release(){
    qCDebug(jtAudio) << "releasing portaudio resources...";
    stop();
    Pa_Terminate();
    qCDebug(jtAudio) << "portaudio terminated!";
}

int PortAudioDriver::getMaxInputs() const{

    if(audioDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(audioDeviceIndex)->maxInputChannels;
    }
    return 0;
}

int PortAudioDriver::getMaxOutputs() const{
    if(audioDeviceIndex != paNoDevice){
        return Pa_GetDeviceInfo(audioDeviceIndex)->maxOutputChannels;
    }
    return 0;
}

void PortAudioDriver::setAudioDeviceIndex(PaDeviceIndex index)
{
    stop();
    this->audioDeviceIndex = index;
}

const char *PortAudioDriver::getAudioDeviceName(int index) const
{
    return Pa_GetDeviceInfo(index)->name;
}


int PortAudioDriver::getDevicesCount() const
{
    return Pa_GetDeviceCount();
}

}
//++++++++++++=

