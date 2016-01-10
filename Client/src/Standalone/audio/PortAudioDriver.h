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

    bool start() override;
    void stop() override;
    void release() override;

    QList<int> getValidSampleRates(int deviceIndex) const override;
    QList<int> getValidBufferSizes(int deviceIndex) const override;

    int getMaxInputs() const override;
    int getMaxOutputs() const override;

    const char *getInputChannelName(unsigned const int index) const override;
    const char *getOutputChannelName(unsigned const int index) const override;

    const char *getAudioDeviceName(int index) const override;
    inline int getAudioDeviceIndex() const
    {
        return audioDeviceIndex;
    }

    void setAudioDeviceIndex(int index) override;

    int getDevicesCount() const override;

    bool canBeStarted() const override;

    bool hasControlPanel() const override;
    void openControlPanel(void *mainWindowHandle) override;

    // portaudio callback function
    friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo *timeInfo,
                                 PaStreamCallbackFlags statusFlags, void *userData);

private:
    bool initPortAudio(int sampleRate, int bufferSize);
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
