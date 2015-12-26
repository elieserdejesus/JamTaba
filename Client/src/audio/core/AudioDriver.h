#pragma once

#include "SamplesBuffer.h"
#include <QObject>
#include <QMutex>

namespace Controller {
class MainController;
}

namespace Audio {
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class ChannelRange
{
private:
    int firstChannel;
    int channelsCount;
public:
    ChannelRange(int firstChannel, int channelsCount);
    ChannelRange();
    inline int getChannels() const
    {
        return channelsCount;
    }

    inline bool isMono() const
    {
        return channelsCount == 1;
    }

    void setToStereo();
    void setToMono();
    inline int getFirstChannel() const
    {
        return firstChannel;
    }

    inline int getLastChannel() const
    {
        return firstChannel + channelsCount - 1;
    }

    inline bool isEmpty() const
    {
        return getChannels() <= 0;
    }
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class AudioDriver : public QObject
{
    Q_OBJECT

signals:
    void sampleRateChanged(int newSampleRate);
    void stopped();
    void started();

public:
    explicit AudioDriver(Controller::MainController *mainController);
    virtual ~AudioDriver();
    virtual void setProperties(int audioDeviceIndex, int firstIn, int lastIn, int firstOut,
                               int lastOut, int sampleRate, int bufferSize);
    virtual void setProperties(int sampleRate, int bufferSize);// used in mac

    virtual void stop() = 0;
    virtual void start() = 0;
    virtual void release() = 0;

    inline ChannelRange getSelectedInputs() const
    {
        return globalInputRange;
    }

    inline ChannelRange getSelectedOutputs() const
    {
        return globalOutputRange;
    }

    virtual inline int getSampleRate() const
    {
        return sampleRate;
    }

    virtual inline int getBufferSize() const
    {
        return bufferSize;
    }

    virtual QList<int> getValidSampleRates(int deviceIndex) const = 0;
    virtual QList<int> getValidBufferSizes(int deviceIndex) const = 0;

    virtual int getMaxInputs() const = 0;// return max inputs for an audio device. My fast track ultra (8 channels) return 8, etc.
    virtual int getMaxOutputs() const = 0;

    virtual const char *getInputChannelName(unsigned const int index) const = 0;
    virtual const char *getOutputChannelName(unsigned const int index) const = 0;

    virtual const char *getAudioDeviceName(int index) const = 0;
    inline const char *getAudioDeviceName() const
    {
        return getAudioDeviceName(audioDeviceIndex);
    }

    virtual int getAudioDeviceIndex() const = 0;
    virtual void setAudioDeviceIndex(int index) = 0;

    virtual int getDevicesCount() const = 0;

    const SamplesBuffer &getOutputBuffer() const
    {
        return *outputBuffer;
    }

    virtual bool canBeStarted() const = 0;

    virtual bool hasControlPanel() const = 0; // ASIO drivers can open control panels to change audio device parameters
    virtual void openControlPanel(void *mainWindowHandle) = 0;
protected:
    ChannelRange globalInputRange;// the range of input channels selected in audio preferences menu
    ChannelRange globalOutputRange;// the range of output channels selected in audio preferences menu

    int audioDeviceIndex;// using same audio device for input and output

    int sampleRate;
    int bufferSize;

    QScopedPointer<SamplesBuffer> inputBuffer;
    QScopedPointer<SamplesBuffer> outputBuffer;

    void recreateBuffers();

    Controller::MainController *mainController;
};

class NullAudioDriver : public AudioDriver
{
    Q_OBJECT // just to use qobject_cast and check if NullAudioDriver is being used in MainController
public:
    NullAudioDriver() :
        AudioDriver(nullptr)
    {
    }

    inline void stop()
    {
    }

    inline void start()
    {
    }

    inline void release()
    {
    }

    inline QList<int> getValidBufferSizes(int) const
    {
        return QList<int>();
    }

    inline QList<int> getValidSampleRates(int) const
    {
        return QList<int>();
    }

    inline int getMaxInputs() const
    {
        return 1;
    }

    inline int getMaxOutputs() const
    {
        return 2;
    }

    inline const char *getInputChannelName(const unsigned int) const
    {
        return "Silence";
    }

    inline const char *getOutputChannelName(const unsigned int) const
    {
        return "Silence";
    }

    inline const char *getAudioDeviceName(int) const
    {
        return "NullAudioDriver";
    }

    inline int getAudioDeviceIndex() const
    {
        return 0;
    }

    inline void setAudioDeviceIndex(int)
    {
    }

    inline int getDevicesCount() const
    {
        return 1;
    }

    inline bool canBeStarted() const
    {
        return true;
    }

    inline bool hasControlPanel() const
    {
        return false;
    }

    inline void openControlPanel(void *)
    {
    }
};
}
