#pragma once

#include "AbstractAudioDriver.h"
#include "portaudio.h"


class PortAudioDriver : public AbstractAudioDriver
{
public:
	PortAudioDriver();
	virtual ~PortAudioDriver();

	virtual void start();
	virtual void stop();
	virtual void release();
    virtual int getMaxInputs() const;
    virtual int getMaxOutputs() const;

    virtual const char* getInputChannelName(unsigned const int index) const;
    virtual const char* getOutputChannelName(unsigned const int index) const;


    PaDeviceIndex getInputDeviceIndex() const;
    void setInputDeviceIndex(PaDeviceIndex index);

    PaDeviceIndex getOutputDeviceIndex() const;
    void setOutputDeviceIndex(PaDeviceIndex index);

    const char* getInputDeviceName(PaDeviceIndex index) const;
    const char* getOutputDeviceName(PaDeviceIndex index) const;
    int getDevicesCount() const;

private:

    float** inputBuffers;//non interleaved buffers
    float** outputBuffers;
    int inputChannels;//total of selected input channels
    int outputChannels;//total of selected output channels (2 channels by default)

	PaStream* paStream;
    PaDeviceIndex inputDeviceIndex;
    PaDeviceIndex outputDeviceIndex;

	void translatePortAudioCallBack(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer);

	//portaudio callback function
	friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);

	void recreateInputBuffers(const int buffersLenght);
	void recreateOutputBuffers(const int buffersLenght);
};

