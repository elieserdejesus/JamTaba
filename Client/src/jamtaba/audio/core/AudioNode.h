#pragma once

#include <QSet>
//#include <cmath>
#include <QMutex>
#include "SamplesBuffer.h"

namespace Midi   {
    class MidiBuffer;
}

namespace Audio{

//class SamplesBuffer;

class AudioNodeProcessor{
public:
    virtual void process(SamplesBuffer& buffer) = 0;
    virtual ~AudioNodeProcessor(){}
};

//++++++++++++++++++++++++++++++++++++++++++++
//# this class is used to apply fade in and fade outs
class FaderProcessor : public AudioNodeProcessor{
private:
    float currentGain;
    float startGain;
    float gainStep;
    int totalSamplesToProcess;
    int processedSamples;
public:
    FaderProcessor(float startGain, float endGain, int samplesToFade);
    virtual void process(SamplesBuffer &buffer);
    bool finished();
    void reset();
};
//++++++++++++++++++++++++++++++++++++++++++++



class AudioNode {

public:
    AudioNode();
    virtual ~AudioNode();

    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    virtual inline void setMuteStatus(bool muted){ this->muted = muted;}
    void inline setSoloStatus(bool soloed){ this->soloed = soloed; }
    inline bool isMuted() const {return muted;}
    inline bool isSoloed() const {return soloed;}

    virtual bool connect(AudioNode &other) ;
    virtual bool disconnect(AudioNode &otherNode);

    void addProcessor(AudioNodeProcessor &newProcessor);
    void removeProcessor(AudioNodeProcessor &processor);

    inline void setGain(float gainValue){
        this->gain = gainValue;
    }

    //virtual inline void setSampleRate(int newSampleRate){this->sampleRate = newSampleRate;}

    virtual int getSampleRate() const = 0;

    inline float getGain() const{return gain;}

    void setPan(float pan);
    inline float getPan() const {return pan;}

    AudioPeak getLastPeak(bool resetPeak=false) const;

    void deactivate();
    inline bool isActivated() const{return activated;}

    virtual bool needResamplingFor(int targetSampleRate) const;
    int getInputResamplingLength(int targetSampleRate, int outFrameLenght) const;

protected:
    QSet<AudioNode*> connections;
    QSet<AudioNodeProcessor*> processors;
    SamplesBuffer internalBuffer;
    mutable Audio::AudioPeak lastPeak;
    QMutex mutex; //used to protected connections manipulation because nodes can be added or removed by different threads
    bool activated; //used to safely remove non activated nodes
    //int sampleRate;
private:
    AudioNode(const AudioNode& other);
    AudioNode& operator=(const AudioNode& other);
    bool muted;
    bool soloed;
    float gain;

    //pan
    float pan;
    float leftGain;
    float rightGain;

	static const double root2Over2;// = 1.414213562373095;// *0.5;
	static const double piOver2;// = 3.141592653589793238463 * 0.5;

    inline void updateGains() {
        double angle = pan * piOver2 * 0.5;
        leftGain = (float) (root2Over2 * (cos(angle) - sin(angle)));
        rightGain = (float) (root2Over2 * (cos(angle) + sin(angle)));
    }

    mutable double resamplingCorrection;
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class OscillatorAudioNode : public AudioNode{

public:
    OscillatorAudioNode(float frequency, int sampleRate);
    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    virtual int getSampleRate() const{return sampleRate;}
private:
    float phase;
    const float phaseIncrement;
    int sampleRate;
};
//+++++++++++++++++

//++++++++++++++++++
class LocalInputAudioNode : public AudioNode{
private:
    bool mono;
    int firstInputIndex;
    //int firstOutputIndex;
public:
    LocalInputAudioNode(int firstInputIndex=0, bool isMono=true);
    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    virtual int getSampleRate() const{return 0;}
};
//++++++++++++++++++++++++
}
