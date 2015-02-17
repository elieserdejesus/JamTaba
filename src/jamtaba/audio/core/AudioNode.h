#pragma once

#include <set>
#include <cmath>

namespace Audio{

class AudioSamplesBuffer;

class AudioNodeProcessor{
public:
    virtual void process(AudioSamplesBuffer& buffer) = 0;
    virtual ~AudioNodeProcessor(){}
};

class FaderProcessor : public AudioNodeProcessor{
private:
    float currentGain;
    float startGain;
    float gainStep;
    int totalSamplesToProcess;
    int processedSamples;
public:
    FaderProcessor(float startGain, float endGain, int samplesToFade);
    virtual void process(AudioSamplesBuffer &buffer);
    bool finished();
    void reset();
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class AudioNode {

public:
    virtual void processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out);
    virtual inline void setMuteStatus(bool muted){ this->muted = muted;}
    void inline setSoloStatus(bool soloed){ this->soloed = soloed; }
    inline bool isMuted() const {return muted;}
    inline bool isSoloed() const {return soloed;}
    AudioNode();
    virtual ~AudioNode();
    virtual bool connect(AudioNode &otherNode) ;
    //virtual bool disconnect(const AudioNode& otherNode);

    void addProcessor(AudioNodeProcessor &newProcessor);

    inline void setGain(float gainValue){
        this->gain = gainValue;
    }

    inline float getGain() const{return gain;}

    void setPan(float pan);
    inline float getPan() const {return pan;}

    inline float getLastPeakLeft() const{return lastPeaks[0];}
    inline float getLastPeakRight() const{return lastPeaks[1];}
    inline float getLastPeak() const {return std::max(lastPeaks[0], lastPeaks[1]);}
protected:
    std::set<AudioNode*> connections;
    std::set<AudioNodeProcessor*> processors;
    AudioSamplesBuffer* internalBuffer;
    float lastPeaks[2];

private:
    bool muted;
    bool soloed;
    float gain;

    //pan
    float leftGain;
    float rightGain;
    float pan;


	static const double root2Over2;// = 1.414213562373095;// *0.5;
	static const double piOver2;// = 3.141592653589793238463 * 0.5;

    inline void updateGains() {
        double angle = pan * piOver2 * 0.5;
        leftGain = (float) (root2Over2 * (cos(angle) - sin(angle)));
        rightGain = (float) (root2Over2 * (cos(angle) + sin(angle)));
    }
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class OscillatorAudioNode : public AudioNode{

public:
    OscillatorAudioNode(float frequency, int sampleRate);
    virtual void processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out);

private:
    float phase;
    const float phaseIncrement;
};
//+++++++++++++++++

class MainOutputAudioNode : public AudioNode
{
public:
    MainOutputAudioNode() {}

    // AudioNode interface
public:
    void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
};
//++++++++++++++++++
class LocalInputAudioNode : public AudioNode{
private:
    bool mono;
    int firstInputIndex;
    //int firstOutputIndex;
public:
    LocalInputAudioNode(int firstInputIndex=0, bool isMono=true);
    virtual void processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out);
};
//++++++++++++++++++++++++
}
