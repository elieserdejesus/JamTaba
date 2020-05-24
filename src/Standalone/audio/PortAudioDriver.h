#ifndef PORT_AUDIO_DRIVER_H
#define PORT_AUDIO_DRIVER_H

#include "audio/core/AudioDriver.h"
#include "portaudio.h"

namespace audio {

class PortAudioDriver : public AudioDriver
{

public:
    PortAudioDriver(controller::MainController *mainController,
                    QString audioInputDevice, QString audioOutputDevice,
                    int firstInputIndex, int lastInputIndex, int firstOutputIndex,
                    int lastOutputIndex, int sampleRate, int bufferSize);

    virtual ~PortAudioDriver();

    bool start() override;
    void stop(bool refreshDevicesList = false) override;
    void release() override;

    QList<int> getValidSampleRates(int deviceIndex) const override;
    QList<int> getValidBufferSizes(int deviceIndex) const override;

    int getMaxInputs() const override;
    int getMaxOutputs() const override;

    QString getInputChannelName(unsigned const int index) const override;
    QString getOutputChannelName(unsigned const int index) const override;

    QString getAudioInputDeviceName(int index = CurrentAudioDeviceSelection) const override;
    QString getAudioOutputDeviceName(int index = CurrentAudioDeviceSelection) const override;
    QString getAudioDeviceInfo(int index, unsigned& nIn, unsigned& nOut) const override;

    int getAudioInputDeviceIndex() const override;
    int getAudioOutputDeviceIndex() const override;

    void setAudioInputDeviceIndex(int index) override;
    void setAudioOutputDeviceIndex(int index) override;

    int getDevicesCount() const override;

    bool canBeStarted() const override;

    bool hasControlPanel() const override;
    void openControlPanel(void *mainWindowHandle) override;

    // portaudio callback function
    friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
                                 unsigned long framesPerBuffer,
                                 const PaStreamCallbackTimeInfo *timeInfo,
                                 PaStreamCallbackFlags statusFlags, void *userData);

protected:
     QStringList getDeviceNames() const;

private:
    bool initPortAudio(int sampleRate, int bufferSize);
    PaStream *paStream;
    void translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer);

    void changeInputSelection(int firstInputChannelIndex, int inputChannelCount);

    void configureHostSpecificInputParameters(PaStreamParameters &inputParameters);
    void configureHostSpecificOutputParameters(PaStreamParameters &outputParameters);

    void releaseHostSpecificParameters(const PaStreamParameters &inputParameters,
                                       const PaStreamParameters &outputParameters);

    void ensureInputRangeIsValid();
    void ensureOutputRangeIsValid();

    void preInitializePortAudioStream(PaStream *stream);

    const bool useSystemDefaultDevices;

};

} // namespace

#endif
