#ifndef PORT_AUDIO_DRIVER_H
#define PORT_AUDIO_DRIVER_H

#include "audio/core/AudioDriver.h"
#include "portaudio.h"

namespace Audio {
class PortAudioDriver : public AudioDriver
{
public:
    PortAudioDriver(Controller::MainController *mainController, int audioDeviceIndex,
                    int firstInputIndex, int lastInputIndex, int firstOutputIndex,
                    int lastOutputIndex, int sampleRate, int bufferSize);
    virtual ~PortAudioDriver();

    virtual void start();
    virtual void stop();
    virtual void release();

    virtual QList<int> getValidSampleRates(int deviceIndex) const;
    virtual QList<int> getValidBufferSizes(int deviceIndex) const;

    virtual int getMaxInputs() const;
    virtual int getMaxOutputs() const;

    virtual const char *getInputChannelName(unsigned const int index) const;
    virtual const char *getOutputChannelName(unsigned const int index) const;

    virtual const char *getAudioDeviceName(int index) const;
    inline int getAudioDeviceIndex() const
    {
        return audioDeviceIndex;
    }

    virtual void setAudioDeviceIndex(int index);

    virtual int getDevicesCount() const;

    virtual bool canBeStarted() const;

    virtual bool hasControlPanel() const;
    virtual void openControlPanel(void *mainWindowHandle);

    // portaudio callback function
    friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo *timeInfo,
                                 PaStreamCallbackFlags statusFlags, void *userData);

private:
    void initPortAudio(int sampleRate, int bufferSize);
    PaStream *paStream;
    void translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer);

    void changeInputSelection(int firstInputChannelIndex, int inputChannelCount);

    void configureHostSpecificInputParameters(PaStreamParameters &inputParameters);
    void configureHostSpecificOutputParameters(PaStreamParameters &outputParameters);

    void releaseHostSpecificParameters(const PaStreamParameters &inputParameters,
                                       const PaStreamParameters &outputParameters);
};
}

#endif
