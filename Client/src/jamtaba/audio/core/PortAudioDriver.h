#pragma once

#include "AudioDriver.h"
#include "portaudio.h"

namespace Audio{

class PortAudioDriver : public AudioDriver
{
public:
    PortAudioDriver(AudioDriverListener* audioDriverListener, int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize);
    PortAudioDriver(AudioDriverListener* audioDriverListener);
    virtual ~PortAudioDriver();

	virtual void start();
	virtual void stop();
	virtual void release();

    virtual int getMaxInputs() const;
    virtual int getMaxOutputs() const;

    virtual const char* getInputChannelName(unsigned const int index) const;
    virtual const char* getOutputChannelName(unsigned const int index) const;

    virtual const char* getInputDeviceName(int index) const;
    virtual const char* getOutputDeviceName(int index) const;

//    virtual inline int getInputs() const {return inputChannels;}
//    virtual inline int getFirstInput() const {return firstInputIndex;}
//    virtual inline int getOutputs() const {return outputChannels;}
//    virtual inline int getFirstOutput() const {return firstOutputIndex;}
//    virtual inline int getSampleRate() const {return sampleRate;}
//    virtual inline int getBufferSize() const {return bufferSize;}

    virtual int getInputDeviceIndex() const;
    virtual void setInputDeviceIndex(int index);
    virtual int getOutputDeviceIndex() const;
    virtual void setOutputDeviceIndex(int index);

    virtual int getDevicesCount() const;


    //portaudio callback function
    friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
        unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags,
        void *userData);

private:
    void initPortAudio(int inputDeviceIndex, int outputDeviceIndex, int firstInputIndex, int lastInputIndex, int firstOutputIndex, int lastOutputIndex, int sampleRate, int bufferSize);
	PaStream* paStream;
    void translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer);

};



}

