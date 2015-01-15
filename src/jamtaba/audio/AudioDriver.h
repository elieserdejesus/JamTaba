#pragma once
#include <vector>

class AudioDriverListener;

class AudioDriver
{
public:
	//AudioDriver(){}
	virtual ~AudioDriver(){}

	virtual void addListener(AudioDriverListener& l) = 0;

	virtual void removeListener(AudioDriverListener& l) = 0;

    /*  this version receive just one deviceIndex (ASIO). The overload version receive input and output device indexes.  */
    virtual void setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize) = 0;
    virtual void setProperties(int inputDeviceIndex, int outputDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize) = 0;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual void release() = 0;
    //virtual void initialize() = 0;

    virtual int getMaxInputs() const = 0;

    virtual int getMaxOutputs() const = 0;

    virtual int getInputs() = 0;
    virtual int getFirstInput() = 0;
    virtual int getOutputs() = 0;
    virtual int getFirstOutput() = 0;

    virtual const char* getInputChannelName(unsigned const int channelIndex) const = 0;

    virtual const char* getOutputChannelName(unsigned const int channelIndex) const = 0;

};
//++++++++++++++++++++++++++++++
class AbstractAudioDriver : public AudioDriver
{


public:
    AbstractAudioDriver();
    virtual ~AbstractAudioDriver(){}
    virtual void addListener(AudioDriverListener& l);
    virtual void removeListener(AudioDriverListener& l);
    virtual void setProperties(int deviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);
    virtual void setProperties(int inputDeviceIndex, int outputDeviceIndex, int firstIn, int lastIn, int firstOut, int lastOut, int sampleRate, int bufferSize);

    virtual inline int getInputs(){return inputChannels;}
    virtual inline int getFirstInput(){return firstInputIndex;}
    virtual inline int getOutputs(){return outputChannels;}
    virtual inline int getFirstOutput(){return firstOutputIndex;}
    //virtual void initialize(){}

protected:
    static const int BUFFERS_LENGHT = 1024;

    float** inputBuffers;//non interleaved buffers
    float** outputBuffers;
    int inputChannels;//total of selected input channels
    int outputChannels;//total of selected output channels (2 channels by default)
    int inputDeviceIndex;//index of selected device index. In ASIO the inputDeviceIndex and outputDeviceIndex are equal.
    int outputDeviceIndex;
    int firstInputIndex;
    int firstOutputIndex;
    int sampleRate;
    int bufferSize;

    std::vector<AudioDriverListener*> listeners;

    void fireDriverCallback(float** in, float** out, const int samplesToProcess);
    void fireDriverStarted() const;
    void fireDriverStopped() const;
    void fireDriverException(const char* msg) const;

    void recreateInputBuffers(const int buffersLenght);
    void recreateOutputBuffers(const int buffersLenght);
};
