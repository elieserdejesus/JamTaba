#include "PortAudioDriver.h"
#include <QDebug>
#include <QtGlobal>

#include "portaudio.h"
#include "audio/core/SamplesBuffer.h"
#include "persistence/Settings.h"
#include "MainController.h"
#include "log/Logging.h"

#include <stdexcept>
#include <algorithm>
#include <cstring>

/*
 * This file contain the platform independent PortAudio code. The platform specific
 * code are in WindowsPortAudioDriver.cpp, MacPortAudioDriver.cpp and in future LinuxPortAudioDriver.cpp.
*/

namespace audio
{

QStringList PortAudioDriver::getDeviceNames() const
{
    QStringList devicesNames;

    auto deviceCount = Pa_GetDeviceCount();
    for (int i = 0; i < deviceCount; ++i) {
        auto deviceInfo = Pa_GetDeviceInfo(i);
        devicesNames << QString(deviceInfo->name);
    }

    return devicesNames;
}

bool PortAudioDriver::canBeStarted() const
{
    if (useSystemDefaultDevices) {
        return     Pa_GetDefaultInputDevice()  != paNoDevice
                && Pa_GetDefaultOutputDevice() != paNoDevice; // we need output
    }

    return     audioInputDeviceIndex  != paNoDevice
            && audioOutputDeviceIndex != paNoDevice;
}

int PortAudioDriver::getAudioInputDeviceIndex() const
{
    return useSystemDefaultDevices ? Pa_GetDefaultInputDevice() :audioInputDeviceIndex;
}

int PortAudioDriver::getAudioOutputDeviceIndex() const
{
    return useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() :audioOutputDeviceIndex;
}


bool PortAudioDriver::initPortAudio(int sampleRate, int bufferSize)
{

    paStream = nullptr;// inputBuffer = outputBuffer = NULL;

    // check for invalid audio device index
    if (!useSystemDefaultDevices) {
        if (audioInputDeviceIndex < 0 ||  audioInputDeviceIndex  >= Pa_GetDeviceCount() ) {
            qCDebug(jtAudio) << "Trying to use default audio device to input";
            audioInputDeviceIndex = Pa_GetDefaultInputDevice();
            if (audioInputDeviceIndex == paNoDevice) {
                audioInputDeviceIndex = Pa_GetDefaultInputDevice();
            }
        }

        if (audioOutputDeviceIndex < 0 ||  audioOutputDeviceIndex  >= Pa_GetDeviceCount() ) {
            qCDebug(jtAudio) << "Trying to use default audio device to output";
            audioOutputDeviceIndex = Pa_GetDefaultOutputDevice();
            if (audioOutputDeviceIndex == paNoDevice) {
                audioOutputDeviceIndex = Pa_GetDefaultOutputDevice();
            }
        }
    }

    ensureInputRangeIsValid();
    ensureOutputRangeIsValid();

    // set sample rate
    this->sampleRate = (sampleRate >= 44100 && sampleRate <= 192000) ? sampleRate : 44100;
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioOutputDeviceIndex;

    if (device != paNoDevice) { // avoid query sample rates in a invalid device
        QList<int> validSampleRates = getValidSampleRates(device);
        if (this->sampleRate > validSampleRates.last()) {
            this->sampleRate = validSampleRates.last(); // use the max supported sample rate
        }
    }

    this->bufferSize = bufferSize;
    if (device != paNoDevice) {
        QList<int> validBufferSizes = getValidBufferSizes(device);
        if (!validBufferSizes.isEmpty()) {
            if (this->bufferSize < validBufferSizes.first()) {
                this->bufferSize = validBufferSizes.first(); // use the minimum supported buffer size
            }
            if (this->bufferSize > validBufferSizes.last()) {
                this->bufferSize = validBufferSizes.last(); // use the max supported buffer size
            }
        }
    }
    return true;
}

void PortAudioDriver::ensureOutputRangeIsValid()
{
    // check if outputs are valid
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioOutputDeviceIndex;
    if (device != paNoDevice) {
        int outputsCount = globalOutputRange.getChannels();
        int maxOutputs = getMaxOutputs();
        if (outputsCount > maxOutputs || globalOutputRange.getFirstChannel() >= maxOutputs || outputsCount <= 0) {
            const PaDeviceInfo* info = Pa_GetDeviceInfo(device);
            if (info)
                globalOutputRange = ChannelRange(info->defaultLowOutputLatency, std::min(2, info->maxOutputChannels));
        }
    }
}

void PortAudioDriver::ensureInputRangeIsValid()
{
    // check if inputs are valid for selected device
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioInputDeviceIndex;
    if (device != paNoDevice) {
        int inputsCount = globalInputRange.getChannels();
        int maxInputs = getMaxInputs();
        if (inputsCount > maxInputs || globalInputRange.getFirstChannel() >= maxInputs || inputsCount <= 0 ) {
            // const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(inputDeviceIndex);
            globalInputRange = ChannelRange( 0, std::min(maxInputs, 1));
        }
    }
}

PortAudioDriver::~PortAudioDriver()
{
    qCDebug(jtAudio) << "PortAudioDriver destructor";
}

// this method just convert portaudio void* inputBuffer to a float[][] buffer, and do the same for outputs
void PortAudioDriver::translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer)
{
    const uint bytesToProcess = framesPerBuffer * sizeof(float);

    // prepare buffers and expose then to application process
    inputBuffer.setFrameLenght(framesPerBuffer);
    outputBuffer.setFrameLenght(framesPerBuffer);
    if (!globalInputRange.isEmpty()) {
        float **inputs = (float**)in;
        int inputChannels = globalInputRange.getChannels();
        for (int c = 0; c < inputChannels; c++) {
            std::memcpy(inputBuffer.getSamplesArray(c), inputs[c], bytesToProcess);
        }
    }
    else {
        inputBuffer.zero();
    }

    outputBuffer.zero();


    // all application audio processing is computed here
    if (mainController) {
        mainController->process(inputBuffer, outputBuffer, sampleRate);
    }

    // convert application output buffers to portaudio format
    float **outputs = static_cast<float**>(out);
    int outputChannels = globalOutputRange.getChannels();
    for (int c = 0; c < outputChannels; c++){
        std::memcpy(outputs[c], outputBuffer.getSamplesArray(c), bytesToProcess);
    }
}

// friend function, receive the pointer to PortAudioDriver instance in userData param
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
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioOutputDeviceIndex;
    if (device == paNoDevice) return false;
    device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioInputDeviceIndex;
    if (device == paNoDevice) return false;

    stop();

    if (!useSystemDefaultDevices) {
        qCInfo(jtAudio) << "Starting Input  portaudio driver using" << getAudioInputDeviceName(audioInputDeviceIndex) << " as device.";
        qCInfo(jtAudio) << "Starting Output portaudio driver using" << getAudioInputDeviceName(audioOutputDeviceIndex) << " as device.";
    }
    else {
        qCDebug(jtAudio) << "Starting portaudio using" << getAudioInputDeviceName() << " as input device.";
        qCDebug(jtAudio) << "Starting portaudio using" << getAudioOutputDeviceName() << " as output device.";
    }

    ensureInputRangeIsValid();
    ensureOutputRangeIsValid();

    recreateBuffers(); //adjust the input and output buffers channels

    unsigned long framesPerBuffer = bufferSize; // paFramesPerBufferUnspecified;
    qCDebug(jtAudio) << "Starting portaudio using" << framesPerBuffer << " as buffer size.";
    PaSampleFormat sampleFormat = paFloat32 | paNonInterleaved;

    PaStreamParameters inputParams;
    inputParams.channelCount = globalInputRange.getChannels();
    inputParams.device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioInputDeviceIndex;
    inputParams.sampleFormat = sampleFormat;
    inputParams.suggestedLatency = 0;//computeSuggestedLatency(sampleRate, bufferSize);// Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowOutputLatency;
    inputParams.hostApiSpecificStreamInfo = NULL;

    configureHostSpecificInputParameters(inputParams); // this can be different in different operational systems

    //+++++++++ OUTPUT
    PaStreamParameters outputParams;
    outputParams.channelCount = globalOutputRange.getChannels();// */outputChannels;
    outputParams.device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioOutputDeviceIndex;
    outputParams.sampleFormat = sampleFormat;
    outputParams.suggestedLatency = 0; //computeSuggestedLatency(sampleRate, bufferSize);//  Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = NULL;

    configureHostSpecificOutputParameters(outputParams);

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    bool willUseInputParams = !globalInputRange.isEmpty();
    if (willUseInputParams) {
        qCDebug(jtAudio) << "Trying initialize portaudio using inputParams and samplerate=" << sampleRate;
    }
    else {
        qCDebug(jtAudio) << "Trying initialize portaudio WITHOUT inputParams because globalInputRange is empty!";
        qCDebug(jtAudio) << "Detected inputs for " << getAudioInputDeviceName(audioInputDeviceIndex) << ":" << getMaxInputs();
    }


    if (globalOutputRange.isEmpty())
        return false;

    // test if output format is supported
    PaError error =  Pa_IsFormatSupported(nullptr, &outputParams, sampleRate);
    if (error != paNoError) {
        qCritical() << "unsuported output format: " <<
                       Pa_GetErrorText(error) <<
                       "sampleRate: " << sampleRate <<
                       "channels: " << outputParams.channelCount << endl;
        this->audioOutputDeviceIndex = paNoDevice;
        releaseHostSpecificParameters(inputParams, outputParams);
        return false;
    }


    // test if input format is supported
    if (!globalInputRange.isEmpty()) {
        error =  Pa_IsFormatSupported(&inputParams, nullptr, sampleRate);
        if (error != paNoError) {
            qCritical() << "unsuported input format: " <<
                           Pa_GetErrorText(error) <<
                           "sampleRate: " << sampleRate <<
                           "channels: " << inputParams.channelCount << endl;
            this->audioInputDeviceIndex = paNoDevice;
            releaseHostSpecificParameters(inputParams, outputParams);
            return false;
        }
    }

    paStream = NULL;
    error = Pa_OpenStream(&paStream,
                          (!globalInputRange.isEmpty()) ? (&inputParams) : NULL,
                          &outputParams,
                          sampleRate,
                          framesPerBuffer,
                          paNoFlag,
                          portaudioCallBack,
                          (void*)this); // I'm passing 'this' to portaudio, so I can run methods inside the callback function

    if (error != paNoError) {
        releaseHostSpecificParameters(inputParams, outputParams);
        return false;
    }
    if (paStream != NULL) {
        preInitializePortAudioStream(paStream);
        error = Pa_StartStream(paStream);
        if (error != paNoError) {
            releaseHostSpecificParameters(inputParams, outputParams);
            return false;
        }
    }
    qCDebug(jtAudio) << "Portaudio driver started ok!";
    emit started();

    releaseHostSpecificParameters(inputParams, outputParams);

    return true;
}

QList<int> PortAudioDriver::getValidSampleRates(int deviceIndex) const
{
    PaStreamParameters outputParams;
    outputParams.channelCount = 1;
    outputParams.device = deviceIndex;
    outputParams.sampleFormat = paFloat32 | paNonInterleaved;
    const PaDeviceInfo *dev = Pa_GetDeviceInfo(deviceIndex);
    if(dev)
    { outputParams.suggestedLatency = dev->defaultLowOutputLatency; }
    outputParams.hostApiSpecificStreamInfo = NULL;
    QList<int> validSRs;
    validSRs.append(44100);
    validSRs.append(48000);
    validSRs.append(96000);
    validSRs.append(192000);
    for (int t = validSRs.size()-1; t >= 0; --t) {
        int sampleRate = validSRs.at(t);
        PaError error =  Pa_IsFormatSupported(  NULL, &outputParams, sampleRate);
        if (error != paNoError) {
            validSRs.removeAt(t);
        }
        else {
            break; // test bigger sample rates first, and when the first big sample rate pass the test assume all other small sample rates are ok
        }
    }
    return validSRs;
}

void PortAudioDriver::stop(bool refreshDevicesList)
{

    if (paStream != NULL) {
        if (!Pa_IsStreamStopped(paStream)) {
            qCDebug(jtAudio) << "Stopping portaudio driver ...";
            PaError error = Pa_CloseStream(paStream);
            if (error != paNoError) {
                qCritical() << "   Error closing portaudio stream: " << Pa_GetErrorText(error);
            }
            emit stopped();
            qCDebug(jtAudio) << "Portaudio driver stoped!";
        }
    }
    if (refreshDevicesList) {
        qCDebug(jtAudio) << "   Refreshing portaudio devices list";
        Pa_Terminate(); // terminate and reinitialize to refresh portaudio internal devices list
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
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : audioInputDeviceIndex;

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(device);
    if (deviceInfo)
        return deviceInfo->maxInputChannels;

    return 0;
}

int PortAudioDriver::getMaxOutputs() const
{
    PaDeviceIndex device = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : audioOutputDeviceIndex;

    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(device);
    if (deviceInfo)
        return deviceInfo->maxOutputChannels;

    return 0;
}

void PortAudioDriver::setAudioInputDeviceIndex(PaDeviceIndex index)
{
    stop();
    audioInputDeviceIndex = !useSystemDefaultDevices ? index: paNoDevice;
}

void PortAudioDriver::setAudioOutputDeviceIndex(PaDeviceIndex index)
{
    stop();
    audioOutputDeviceIndex = !useSystemDefaultDevices ? index: paNoDevice;
}

QString PortAudioDriver::getAudioInputDeviceName(int index) const
{
    index = useSystemDefaultDevices ? Pa_GetDefaultInputDevice() : index == CurrentAudioDeviceSelection ? audioInputDeviceIndex : index;
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(index);
    return deviceInfo? deviceInfo->name : "Error!";
}

QString PortAudioDriver::getAudioOutputDeviceName(int index) const
{
    index = useSystemDefaultDevices ? Pa_GetDefaultOutputDevice() : index == CurrentAudioDeviceSelection ? audioOutputDeviceIndex : index;
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(index);
    return deviceInfo? deviceInfo->name : "Error!";
}

QString PortAudioDriver::getAudioDeviceInfo(int index, unsigned& nIn, unsigned& nOut) const
{
    const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(index);
    nIn =  deviceInfo ? (unsigned) deviceInfo->maxInputChannels : 0;
    nOut = deviceInfo ? (unsigned) deviceInfo->maxOutputChannels : 0;
    return deviceInfo ? deviceInfo->name : "Error!";
}

int PortAudioDriver::getDevicesCount() const
{
    return Pa_GetDeviceCount();
}

} // namespace
