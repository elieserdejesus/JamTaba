#pragma once
#include <set>

class AudioSamplesBuffer;

class AudioNode {

public:
    virtual void processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out);
    virtual inline void setMuteStatus(bool muted){ this->muted = muted;}
    void inline setSoloStatus(bool soloed){ this->soloed = soloed; }
    inline bool isMuted(){return muted;}
    inline bool isSoloed(){return soloed;}
    virtual ~AudioNode(){}
    virtual bool connect(AudioNode *otherNode);
    //virtual bool disconnect(const AudioNode& otherNode);

protected:
    std::set<AudioNode*> connections;

private:
    bool muted;
    bool soloed;
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
class GainNode : public AudioNode{
private:
    float gain;
    AudioSamplesBuffer* internalBuffer;
public:
    GainNode(float initialGain=1);
    ~GainNode();
    void setGain(float gain);
    inline float getGain();
    virtual void processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out);
};
