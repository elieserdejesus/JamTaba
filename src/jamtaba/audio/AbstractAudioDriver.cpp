#include "AbstractAudioDriver.h"
#include <vector>
#include "AudioDriverListener.h"

AbstractAudioDriver::AbstractAudioDriver(){
	//
}

void AbstractAudioDriver::addListener(AudioDriverListener& l){
	this->listeners.push_back(&l);
}

void AbstractAudioDriver::removeListener(AudioDriverListener& l){
	std::vector< AudioDriverListener*>::iterator it = this->listeners.begin();
	while (it != this->listeners.end()){
		if ((*it) == &l){
			this->listeners.erase(it);
		}
		else{
			it++;
		}
	}
}

void AbstractAudioDriver::fireDriverStarted() const{
	std::vector< AudioDriverListener*>::const_iterator it = listeners.begin();
	for (; it != listeners.end(); it++){
		(*it)->driverStarted();
	}
}

void AbstractAudioDriver::fireDriverStopped() const{
	std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
	for (; it != listeners.end(); it++){
		(*it)->driverStopped();
	}
}

void AbstractAudioDriver::fireDriverException(const char* msg) const{
	std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
	for (; it != listeners.end(); it++){
		(*it)->driverException(msg);
	}
}

void AbstractAudioDriver::fireDriverCallback(float** in, float** out, const int samplesToProcess){
	std::vector<AudioDriverListener*>::const_iterator it = listeners.begin();
	for (; it != listeners.end(); it++){
		(*it)->processCallBack(in, out, samplesToProcess);
	}
}



