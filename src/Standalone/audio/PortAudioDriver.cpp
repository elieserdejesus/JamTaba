#include "PortAudioDriver.h"
#include <QDebug>
#include <stdexcept>
#include <algorithm>
#include "portaudio.h"
#include "audio/core/SamplesBuffer.h"
#include "persistence/Settings.h"
#include "MainController.h"
#include "log/Logging.h"
#include <QtGlobal>

/*
 * This file contain the platform independent PortAudio code. The platform specific
 * code are in WindowsPortAudioDriver.cpp, MacPortAudioDriver.cpp and in future LinuxPortAudioDriver.cpp.
*/

namespace Audio
{

bool PortAudioDriver::canBeStarted() const
{
    if (useSystemDefaultDevices) {
        return Pa_GetDefaultOutputDevice() != paNoDevice; // we need output
    }

    return audioDeviceIndex != paNoDevice;
}

bool PortAudioDriver::initPortAudio(int sampleRate, int bufferSize)
{
    qCInfo(jtAudio) << "initializing portaudio...";
    PaError error = Pa_Initialize();
    if (error != paNoError){
        qCCritical(jtAudio) << "ERROR initializing portaudio:" << Pa_GetErrorText(error);
        return false;
    }
    paStream = nullptr;// inputBuffer = outputBuffer = NULL;

    //check for invalid audio device index
    if (!useSystemDefaultDevices) {
        if(audioDeviceIndex < 0 || audioDeviceIndex >= Pa_GetDeviceCount()){
            audioDeviceIndex = Pa_GetDefaultInputDevice();
            if(audioDeviceIndex == paNoDevice){
                audioDeviceIndex = Pa_GetDefaultOutputDevice();
            }
        }
    }

    ensureInputRangeIsValid();
    ensureOutputRangeIsValid();

    //set sample rate
    this->sampleRate = (sampleRate >= 44100 && sampleRate <= 192000) ? sampleRate : 44100;
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;
    if(device != paNoDevice){//avoid query sample rates in a invalid device
        QList<int> validSampleRates = getValidSampleRates(device);
        if(this->sampleRate > validSampleRates.last()){
            this->sampleRate = validSampleRates.last();//use the max supported sample rate
        }
    }

    this->bufferSize = bufferSize;
    if(device != paNoDevice){
        QList<int> validBufferSizes = getValidBufferSizes(device);
        if (!validBufferSizes.isEmpty()) {
            if(this->bufferSize < validBufferSizes.first()){
                this->bufferSize = validBufferSizes.first();//use the minimum supported buffer size
            }
            if(this->bufferSize > validBufferSizes.last()){
                this->bufferSize = validBufferSizes.last();//use the max supported buffer size
            }
        }
    }
    return true;
}

void PortAudioDriver::ensureOutputRangeIsValid()
{
    //check if outputs are valid
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;
    if(device != paNoDevice){
        int outputsCount = globalOutputRange.getChannels();
        int maxOutputs = getMaxOutputs();
        if(outputsCount > maxOutputs || globalOutputRange.getFirstChannel() >= maxOutputs || outputsCount <= 0){
            const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
            if (info)
                globalOutputRange = ChannelRange(info->defaultLowOutputLatency, std::min(2, info->maxOutputChannels));
        }
    }
}

void PortAudioDriver::ensureInputRangeIsValid()
{
    //check if inputs are valid for selected device
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioDeviceIndex;
    if(device != paNoDevice){
        int inputsCount = globalInputRange.getChannels();
        int maxInputs = getMaxInputs();
        if(inputsCount > maxInputs || globalInputRange.getFirstChannel() >= maxInputs || inputsCount <= 0 ){
            //const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(inputDeviceIndex);
            globalInputRange = ChannelRange( 0, std::min(maxInputs, 1));
        }
    }
}

PortAudioDriver::~PortAudioDriver()
{
    qCDebug(jtAudio) << "destructor PortAudioDriver";
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//this method just convert portaudio void* inputBuffer to a float[][] buffer, and do the same for outputs
void PortAudioDriver::translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer)
{
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


bool PortAudioDriver::start()
{

    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;

    if(device == paNoDevice){
        return false;
    }

    stop();

    if (!useSystemDefaultDevices) {
        qCInfo(jtAudio) << "Starting portaudio driver using" << getAudioInputDeviceName(audioDeviceIndex) << " as device.";
    }
    else {
        qCDebug(jtAudio) << "Starting portaudio using" << getAudioInputDeviceName() << " as input device.";
        qCDebug(jtAudio) << "Starting portaudio using" << getAudioOutputDeviceName() << " as output device.";
    }

    ensureInputRangeIsValid();
    ensureOutputRangeIsValid();

    recreateBuffers();//adjust the input and output buffers channels

    unsigned long framesPerBuffer = bufferSize;// paFramesPerBufferUnspecified;
    qCInfo(jtAudio) << "Starting portaudio driver using" << framesPerBuffer << " as buffer size.";
    PaSampleFormat sampleFormat = paFloat32;// | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = globalInputRange.getChannels();// maxInputChannels;//*/ inputChannels;
    inputParams.device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);// Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    configureHostSpecificInputParameters(inputParams);//this can be different in different operational systems

    //+++++++++ OUTPUT
    PaStreamParameters outputParams;
    outputParams.channelCount = globalOutputRange.getChannels();// */outputChannels;
    outputParams.device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;
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
        qCInfo(jtAudio) << "Detected inputs for " << getAudioInputDeviceName(audioDeviceIndex) << ":" << getMaxInputs();
    }


    if(globalOutputRange.isEmpty())
        return false;

    // test if output format is supported
    PaError error =  Pa_IsFormatSupported(nullptr, &outputParams, sampleRate);
    if(error != paNoError){
        qCritical() << "unsuported output format: " <<
                       Pa_GetErrorText(error) <<
                       "sampleRate: " << sampleRate <<
                       "channels: " << outputParams.channelCount << endl;
        this->audioDeviceIndex = paNoDevice;
        releaseHostSpecificParameters(inputParams, outputParams);
        return false;
    }


    // test if input format is supported
    if(!globalInputRange.isEmpty()){
        error =  Pa_IsFormatSupported(&inputParams, nullptr, sampleRate);
        if(error != paNoError){
            qCritical() << "unsuported input format: " <<
                           Pa_GetErrorText(error) <<
                           "sampleRate: " << sampleRate <<
                           "channels: " << inputParams.channelCount << endl;
            this->audioDeviceIndex = paNoDevice;
            releaseHostSpecificParameters(inputParams, outputParams);
            return false;
        }
    }

    paStream = NULL;
    error = Pa_OpenStream(&paStream,
                          (!globalInputRange.isEmpty()) ? (&inputParams) : NULL,
                          &outputParams,
                          sampleRate, framesPerBuffer,
                          paNoFlag, portaudioCallBack, (void*)this);//I'm passing this to portaudio, so I can run methods inside the callback function
    if (error != paNoError){
        releaseHostSpecificParameters(inputParams, outputParams);
        return false;
    }
    if(paStream != NULL){
        preInitializePortAudioStream(paStream);
        error = Pa_StartStream(paStream);
        if (error != paNoError){
            releaseHostSpecificParameters(inputParams, outputParams);
            return false;
        }
    }
    qCInfo(jtAudio) << "portaudio driver started ok!";
    emit started();

    releaseHostSpecificParameters(inputParams, outputParams);

    return true;
}

QList<int> PortAudioDriver::getValidSampleRates(int deviceIndex) const
{
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

void PortAudioDriver::stop(bool refreshDevicesList)
{
    if (paStream != NULL){
        if (!Pa_IsStreamStopped(paStream)){
            qCDebug(jtAudio) << "closing portaudio stream";
            PaError error = Pa_CloseStream(paStream);
            if(error != paNoError){
                qCCritical(jtAudio) << "error closing portaudio stream: " << Pa_GetErrorText(error);
            }
            emit stopped();
        }
    }
    if(refreshDevicesList){
        Pa_Terminate(); //terminate and reinitialize to refresh portaudio internal devices list
        Pa_Initialize();
    }
}

void PortAudioDriver::release()
{
    qCDebug(jtAudio) << "releasing portaudio resources...";
    stop();
    Pa_Terminate();
    qCDebug(jtAudio) << "portaudio terminated!";
}

int PortAudioDriver::getMaxInputs() const
{
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioDeviceIndex;

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(device);
    if(deviceInfo)
        return deviceInfo->maxInputChannels;

    return 0;
}

int PortAudioDriver::getMaxOutputs() const
{
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(device);
    if(deviceInfo)
        return deviceInfo->maxOutputChannels;

    return 0;
}

void PortAudioDriver::setAudioDeviceIndex(PaDeviceIndex index)
{
    stop();
    if (!useSystemDefaultDevices)
        this->audioDeviceIndex = index;
    else
        this->audioDeviceIndex = paNoDevice;
}

QString PortAudioDriver::getAudioInputDeviceName(int index) const
{
    if (useSystemDefaultDevices)
        index = Pa_GetDefaultInputDevice();

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(index);
    if (deviceInfo)
        return deviceInfo->name;

    return "Error!";
}

QString PortAudioDriver::getAudioInputDeviceName() const
{
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioDeviceIndex;

    return getAudioInputDeviceName(device);
}

QString PortAudioDriver::getAudioOutputDeviceName(int index) const
{
    if (useSystemDefaultDevices)
        index = Pa_GetDefaultOutputDevice();

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(index);
    if (deviceInfo)
        return deviceInfo->name;

    return "Error!";
}

QString PortAudioDriver::getAudioOutputDeviceName() const
{
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioDeviceIndex;

    return getAudioOutputDeviceName(device);
}

int PortAudioDriver::getDevicesCount() const
{
    return Pa_GetDeviceCount();
}

} // namespace
//++++++++++++=

