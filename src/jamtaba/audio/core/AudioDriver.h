#pragma once

#include "SamplesBuffer.h"
#include <QObject>

namespace Audio{

class AudioDriverListener{
public:
    virtual void process(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out) = 0;
};

class AudioDriver : public QObject
{
    Q_OBJECT

signals:
    void sampleRateChanged(int newSampleRate);//invocado quando acontece alguma mudança na configuração de buffer size, por exemplo
    void stopped();
    void started();
    //void driverException(const char* msg) = 0;

public:
    AudioDriver( AudioDriverListener* audioDriverListener );
    virtual ~AudioDriver(){}
    virtual void setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    virtual void setProperties(int inputDeviceIndex, int outputDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

    virtual void stop() = 0;
    virtual void start() = 0;
    virtual void release() = 0;

    virtual inline int getInputs() const {return inputChannels;}
    virtual inline int getFirstInput() const {return firstInputIndex;}
    virtual inline int getOutputs() const {return outputChannels;}
    virtual inline int getFirstOutput() const {return firstOutputIndex;}
    virtual inline int getSampleRate() const {return sampleRate;}
    virtual inline int getBufferSize() const {return bufferSize;}

    inline int getMaxInputs() const{return maxInputChannels;}
    inline int getMaxOutputs() const{return maxOutputChannels;}

    virtual const char* getInputChannelName(unsigned const int index) const = 0;
    virtual const char* getOutputChannelName(unsigned const int index) const = 0;

    virtual const char* getInputDeviceName(int index) const = 0;
    virtual const char* getOutputDeviceName(int index) const = 0;

    virtual int getInputDeviceIndex() const = 0;
    virtual void setInputDeviceIndex(int index) = 0;
    virtual int getOutputDeviceIndex() const = 0;
    virtual void setOutputDeviceIndex(int index) = 0;

    virtual int getDevicesCount() const = 0;

    const SamplesBuffer& getOutputBuffer() const {return outputBuffer;}
protected:
    int maxInputChannels;
    int maxOutputChannels;
    int inputChannels;//total of selected input channels
    int outputChannels;//total of selected output channels (2 channels by default)
    int inputDeviceIndex;//index of selected device index. In ASIO the inputDeviceIndex and outputDeviceIndex are equal.
    int outputDeviceIndex;
    int firstInputIndex;
    int firstOutputIndex;

    int sampleRate;
    int bufferSize;

    SamplesBuffer inputBuffer;
    SamplesBuffer outputBuffer;

    void recreateBuffers(const int maxInputs, const int maxOutputs);

    AudioDriverListener* audioDriverListener;
};

}
