#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include "SamplesBuffer.h"
#include <QObject>
#include <QMutex>

// Change settings below to experiment with seperate input/output audio devices
#ifdef Q_OS_WIN
    const bool UseSingleAudioIODevice = true;
#else
    const bool UseSingleAudioIODevice = false;
#endif

namespace controller {
class MainController;
}

namespace audio {

class ChannelRange
{

private:

    int firstChannel;
    int channelsCount;

public:

    ChannelRange(int firstChannel, int channelsCount);
    ChannelRange();

    int getChannels() const;

    bool isMono() const;

    void setToStereo();
    void setToMono();
    int getFirstChannel() const;
    int getLastChannel() const;

    bool isEmpty() const;
};


inline int ChannelRange::getFirstChannel() const
{
    return firstChannel;
}

inline int ChannelRange::getLastChannel() const
{
    return firstChannel + channelsCount - 1;
}

inline bool ChannelRange::isEmpty() const
{
    return getChannels() <= 0;
}

inline int ChannelRange::getChannels() const
{
    return channelsCount;
}

inline bool ChannelRange::isMono() const
{
    return channelsCount == 1;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const int CurrentAudioDeviceSelection = -1;

class AudioDriver : public QObject
{
    Q_OBJECT

signals:
    void sampleRateChanged(int newSampleRate);
    void stopped();
    void started();

public:
    explicit AudioDriver(controller::MainController *mainController);
    virtual ~AudioDriver();
    virtual void setProperties(int firstIn, int lastIn, int firstOut, int lastOut);

    virtual void setSampleRate(int newSampleRate);
    virtual void setBufferSize(int newBufferSize);

    virtual void stop(bool refreshDevicesList = false) = 0;
    virtual bool start() = 0;
    virtual void release() = 0;

    int getInputsCount() const;

    int getOutputsCount() const;

    int getFirstSelectedInput() const;

    int getFirstSelectedOutput() const;

    virtual int getSampleRate() const;

    virtual int getBufferSize() const;

    virtual QList<int> getValidSampleRates(int deviceIndex) const = 0;
    virtual QList<int> getValidBufferSizes(int deviceIndex) const = 0;

    virtual int getMaxInputs() const = 0; // return max inputs for an audio device. My fast track ultra (8 channels) return 8, etc.
    virtual int getMaxOutputs() const = 0;

    virtual QString getInputChannelName(unsigned const int index) const = 0;
    virtual QString getOutputChannelName(unsigned const int index) const = 0;

    virtual QString getAudioDeviceInfo(int index, unsigned& nInputs, unsigned& nOutputs) const = 0;

    virtual QString getAudioInputDeviceName(int index = CurrentAudioDeviceSelection) const = 0;
    virtual QString getAudioOutputDeviceName(int index = CurrentAudioDeviceSelection) const = 0;

    virtual int getAudioInputDeviceIndex() const = 0;
    virtual int getAudioOutputDeviceIndex() const = 0;

    virtual void setAudioInputDeviceIndex(int index) = 0;
    virtual void setAudioOutputDeviceIndex(int index) = 0;


    virtual int getDevicesCount() const = 0;

    const SamplesBuffer &getOutputBuffer() const;

    virtual bool canBeStarted() const = 0;

    virtual bool hasControlPanel() const = 0; // ASIO drivers can open control panels to change audio device parameters
    virtual void openControlPanel(void *mainWindowHandle) = 0;

protected:
    ChannelRange globalInputRange; // the range of input channels selected in audio preferences menu
    ChannelRange globalOutputRange; // the range of output channels selected in audio preferences menu

    int audioInputDeviceIndex; // not using same audio device for input and output
    int audioOutputDeviceIndex; // not using same audio device for input and output

    int sampleRate;
    int bufferSize;

    SamplesBuffer inputBuffer;
    SamplesBuffer outputBuffer;

    void recreateBuffers();

    controller::MainController *mainController;
};


inline const SamplesBuffer &AudioDriver::getOutputBuffer() const
{
    return outputBuffer;
}

inline int AudioDriver::getInputsCount() const
{
    return globalInputRange.getChannels();
}

inline int AudioDriver::getOutputsCount() const
{
    return globalOutputRange.getChannels();
}

inline int AudioDriver::getFirstSelectedInput() const
{
    return globalInputRange.getFirstChannel();
}

inline int AudioDriver::getFirstSelectedOutput() const
{
    return globalOutputRange.getFirstChannel();
}

inline int AudioDriver::getSampleRate() const
{
    return sampleRate;
}

inline int AudioDriver::getBufferSize() const
{
    return bufferSize;
}



class NullAudioDriver : public AudioDriver
{
    Q_OBJECT // just to use qobject_cast and check if NullAudioDriver is being used in MainController

public:

    NullAudioDriver() :
        AudioDriver(nullptr)
    {

    }

    void stop(bool) override;

    bool start() override;

    void release() override;

    QList<int> getValidBufferSizes(int) const override;

    QList<int> getValidSampleRates(int) const override;

    int getMaxInputs() const  override;

    int getMaxOutputs() const  override;

    QString getInputChannelName(const unsigned int) const override;

    QString getOutputChannelName(const unsigned int) const override;

    QString getAudioInputDeviceName(int index = CurrentAudioDeviceSelection) const override;

    QString getAudioOutputDeviceName(int index = CurrentAudioDeviceSelection) const override;

    QString getAudioDeviceInfo(int index, unsigned& nIn, unsigned& nOut) const override;

    int getAudioInputDeviceIndex() const override;

    void setAudioInputDeviceIndex(int) override;

    int getAudioOutputDeviceIndex() const override;

    void setAudioOutputDeviceIndex(int) override;

    int getDevicesCount() const override;

    bool canBeStarted() const override;

    bool hasControlPanel() const override;

    void openControlPanel(void *) override;
};


inline void NullAudioDriver::stop(bool)
{
    //
}

inline bool NullAudioDriver::start()
{
    return true;
}

inline void NullAudioDriver::release()
{
    //
}

inline QList<int> NullAudioDriver::getValidBufferSizes(int) const
{
    return QList<int>();
}

inline QList<int> NullAudioDriver::getValidSampleRates(int) const
{
    return QList<int>();
}

inline int NullAudioDriver::getMaxInputs() const
{
    return 1;
}

inline int NullAudioDriver::getMaxOutputs() const
{
    return 2;
}

inline QString NullAudioDriver::getInputChannelName(const unsigned int) const
{
    return "Silence";
}

inline QString NullAudioDriver::getOutputChannelName(const unsigned int) const
{
    return "Silence";
}

inline QString NullAudioDriver::getAudioInputDeviceName(int index) const
{
    return index == CurrentAudioDeviceSelection ? getAudioInputDeviceName(0) : "";
}

inline QString NullAudioDriver::getAudioOutputDeviceName(int index) const
{
    return index == CurrentAudioDeviceSelection ? getAudioOutputDeviceName(0) : "";
}

inline QString NullAudioDriver::getAudioDeviceInfo(int index,unsigned& nIn, unsigned& nOut ) const
{
    nIn = nOut = 1;
    return index==0 ? "" : "";
}

inline int NullAudioDriver::getAudioInputDeviceIndex() const
{
    return 0;
}

inline void NullAudioDriver::setAudioInputDeviceIndex(int)
{
    //
}

inline int NullAudioDriver::getAudioOutputDeviceIndex() const
{
    return 0;
}

inline void NullAudioDriver::setAudioOutputDeviceIndex(int)
{
    //
}

inline int NullAudioDriver::getDevicesCount() const
{
    return 1;
}

inline bool NullAudioDriver::canBeStarted() const
{
    return true;
}

inline bool NullAudioDriver::hasControlPanel() const
{
    return false;
}

inline void NullAudioDriver::openControlPanel(void *)
{
    //
}

} // namespace

#endif
