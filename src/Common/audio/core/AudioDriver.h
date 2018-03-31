#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include "SamplesBuffer.h"
#include <QObject>
#include <QMutex>

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

    virtual QString getAudioInputDeviceName(int index) const = 0;
    virtual QString getAudioInputDeviceName() const = 0;

    virtual QString getAudioOutputDeviceName(int index) const = 0;
    virtual QString getAudioOutputDeviceName() const = 0;

    virtual int getAudioDeviceIndex() const = 0;
    virtual void setAudioDeviceIndex(int index) = 0;

    virtual int getDevicesCount() const = 0;

    const SamplesBuffer &getOutputBuffer() const;

    virtual bool canBeStarted() const = 0;

    virtual bool hasControlPanel() const = 0; // ASIO drivers can open control panels to change audio device parameters
    virtual void openControlPanel(void *mainWindowHandle) = 0;

protected:
    ChannelRange globalInputRange; // the range of input channels selected in audio preferences menu
    ChannelRange globalOutputRange; // the range of output channels selected in audio preferences menu

    int audioDeviceIndex; // using same audio device for input and output

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

    void release();

    QList<int> getValidBufferSizes(int) const;

    QList<int> getValidSampleRates(int) const;

    int getMaxInputs() const;

    int getMaxOutputs() const;

    QString getInputChannelName(const unsigned int) const;

    QString getOutputChannelName(const unsigned int) const;

    QString getAudioInputDeviceName(int) const;

    QString getAudioInputDeviceName() const;

    QString getAudioOutputDeviceName(int) const;

    QString getAudioOutputDeviceName() const;

    int getAudioDeviceIndex() const;

    void setAudioDeviceIndex(int);

    int getDevicesCount() const;

    bool canBeStarted() const;

    bool hasControlPanel() const;

    void openControlPanel(void *);
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

inline QString NullAudioDriver::getAudioInputDeviceName(int) const
{
    return "NullAudioDriver";
}

inline QString NullAudioDriver::getAudioInputDeviceName() const
{
    return getAudioInputDeviceName(0);
}

inline QString NullAudioDriver::getAudioOutputDeviceName(int) const
{
    return "NullAudioDriver";
}

inline QString NullAudioDriver::getAudioOutputDeviceName() const
{
    return getAudioOutputDeviceName(0);
}

inline int NullAudioDriver::getAudioDeviceIndex() const
{
    return 0;
}

inline void NullAudioDriver::setAudioDeviceIndex(int)
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
