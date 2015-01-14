#pragma once

#include "AudioDriver.h"
#include <vector>

class AbstractAudioDriver : public AudioDriver
{

public:
	virtual void addListener(AudioDriverListener& l);
	virtual void removeListener(AudioDriverListener& l);
	AbstractAudioDriver();
	virtual ~AbstractAudioDriver(){}

protected:
	

	std::vector<AudioDriverListener*> listeners;

	void fireDriverCallback(float** in, float** out, const int samplesToProcess);
	void fireDriverStarted() const;
	void fireDriverStopped() const;
	void fireDriverException(const char* msg) const;
};

