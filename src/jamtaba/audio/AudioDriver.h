#pragma once

#include <vector>
#include <memory>

namespace Audio{

class AudioDriverListener;
class AudioNodeProcessor;

class AudioSamplesBuffer{

    friend class AudioNodeProcessor;

private:
    float** samples;
    unsigned int channels;
    unsigned int frameLenght;

    mutable float peaks[2];

    inline bool isMono() const {return channels == 1;}

    inline bool channelIsValid(unsigned int channel){return channel < channels;}
    inline bool sampleIndexIsValid(unsigned int sampleIndex){return sampleIndex < frameLenght;}

    void computePeaks();

public:
    AudioSamplesBuffer(unsigned int channels, const unsigned int MAX_BUFFERS_LENGHT);
    ~AudioSamplesBuffer();

    void applyGain(float gainFactor);

    //overload applyGain used to compute main gain and pan gains in one pass
    void applyGain(float gainFactor, float leftGain, float rightGain);//panValue between [-1, 0, 1] => LEFT, CENTER, RIGHT
    void zero();

    const float *getPeaks() const;//{return peaks[channel];}

    void add(const AudioSamplesBuffer& buffer);
    inline void add(int channel, int sampleIndex, float sampleValue){
        samples[channel][sampleIndex] += sampleValue;
    }

    void set(const AudioSamplesBuffer& buffer);
    inline void set(int channel, int sampleIndex, float sampleValue)
    {
        samples[channel][sampleIndex] = sampleValue;
    }

    inline float get(int channel, int sampleIndex) const
    {
        return samples[channel][sampleIndex];
    }

    //inline int getSampleRate(){ return sampleRate; }
    inline int getFrameLenght() const{ return frameLenght; }
    inline void setFrameLenght(unsigned int newFrameLenght)  {this->frameLenght = newFrameLenght;}
    inline int getChannels() const { return channels; }
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class AudioDriver
{
public:

    static const unsigned int MAX_BUFFERS_LENGHT = 2048;//size of internal audio buffers

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

    virtual int getInputs() const = 0;
    virtual int getFirstInput() const = 0;
    virtual int getOutputs() const = 0;
    virtual int getFirstOutput() const = 0;

    virtual int getInputDeviceIndex() const = 0;
    virtual void setInputDeviceIndex(int index) = 0;

    virtual int getOutputDeviceIndex() const = 0;
    virtual void setOutputDeviceIndex(int index) = 0;

    virtual const char* getInputDeviceName(int index) const = 0;
    virtual const char* getOutputDeviceName(int index) const = 0;
    virtual int getDevicesCount() const = 0;

    virtual const char* getInputChannelName(unsigned const int index) const = 0;
    virtual const char* getOutputChannelName(unsigned const int index) const = 0;

    virtual int getSampleRate() const = 0;
    virtual int getBufferSize() const = 0;
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

    virtual inline int getInputs() const {return inputChannels;}
    virtual inline int getFirstInput() const {return firstInputIndex;}
    virtual inline int getOutputs() const {return outputChannels;}
    virtual inline int getFirstOutput() const {return firstOutputIndex;}
    virtual inline int getSampleRate() const {return sampleRate;}
    virtual inline int getBufferSize() const {return bufferSize;}


    const AudioSamplesBuffer& getOutputBuffer() const {return *outputBuffer;}

protected:
    //float* inputMasks;
    //float* outputMasks;//estou abrindo todos os canais do device selecionado, os canais que não estão selecionados pelo usuário
    //tem as amostras zeradas. Sempre pego cada amostra e multiplico pela máscara. Os canais selecionados são multiplicados por 1, e os
    //não selecionados são multiplicados por ZERO.

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

    AudioSamplesBuffer* inputBuffer;
    AudioSamplesBuffer* outputBuffer;

    std::vector<AudioDriverListener*> listeners;

    void fireDriverCallback(AudioSamplesBuffer& in, AudioSamplesBuffer& out) const;
    void fireDriverStarted() const;
    void fireDriverStopped() const;
    void fireDriverException(const char* msg) const;

    void recreateBuffers(const int buffersLenght, const int maxInputs, const int maxOutputs);
};

class AudioDriverListener
{
public:
    virtual void processCallBack(AudioSamplesBuffer& in, AudioSamplesBuffer& out) = 0;
    virtual void driverParametersChanged() = 0;//invocado quando acontece alguma mudança na configuração de buffer size, por exemplo
    virtual void driverInitialized() = 0;
    virtual void driverStopped() = 0;
    virtual void driverStarted() = 0;
    virtual void driverException(const char* msg) = 0;

    virtual ~AudioDriverListener(){}
    AudioDriverListener(){}
};

class AudioDriverListenerAdapter : public AudioDriverListener
{
public:
    virtual void processCallBack(AudioSamplesBuffer&, AudioSamplesBuffer& ){
//
    };
    virtual void driverParametersChanged(){}//invocado quando acontece alguma mudança na configuração de buffer size, por exemplo
    virtual void driverInitialized(){}
    virtual void driverStopped(){}
    virtual void driverStarted(){}
    virtual void driverException(const char*){}

    virtual ~AudioDriverListenerAdapter(){}
    AudioDriverListenerAdapter() { }
};

}
