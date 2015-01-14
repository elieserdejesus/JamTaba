#pragma once

class AudioDriverListener;

class AudioDriver
{
public:
	//AudioDriver(){}
	virtual ~AudioDriver(){}

	virtual void addListener(AudioDriverListener& l) = 0;

	virtual void removeListener(AudioDriverListener& l) = 0;

	virtual void start() = 0;

	virtual void stop() = 0;

	virtual void release() = 0;

    virtual int getMaxInputs() const = 0;

    virtual int getMaxOutputs() const = 0;

    virtual const char* getInputChannelName(unsigned const int channelIndex) const = 0;

    virtual const char* getOutputChannelName(unsigned const int channelIndex) const = 0;

};

