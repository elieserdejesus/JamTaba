#pragma once

#include "AudioDriver.h"
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

	PaStream* paStream;


    void translatePortAudioCallBack(const void *in, void *out, unsigned long framesPerBuffer);

	//portaudio callback function
	friend int portaudioCallBack(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData);




};

