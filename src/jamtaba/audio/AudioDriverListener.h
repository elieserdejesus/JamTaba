#pragma once

class AudioDriverListener
{
public:
	virtual void processCallBack(float** in, float** out, const int samplesToProcess) = 0;
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
    virtual void processCallBack(float**, float**, const int){
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

