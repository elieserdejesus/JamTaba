#pragma once

#include <QSet>
//#include <cmath>
#include <QMutex>
#include "SamplesBuffer.h"
#include "AudioDriver.h"
#include <QDebug>

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

    //void pushBackDiscardedSamples(const Audio::SamplesBuffer& buffer, unsigned int discardedSamples);

    inline void setGain(float gainValue){
        this->gain = gainValue;
    }

    //virtual inline void setSampleRate(int newSampleRate){this->sampleRate = newSampleRate;}

    virtual int getSampleRate() const = 0;

    inline float getGain() const{
        return gain;
    }

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
    //SamplesBuffer discardedBuffer;//store samples discarded in AudioMixer to avoid loose samples in resampling process
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

    void updateGains();

    //mutable double resamplingCorrection;
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
    int globalFirstInputIndex; //store the first input index selected globally by users in preferences menu
    ChannelRange audioInputRange;
    //store user selected input range. For example, user can choose just the
    //right input channel (index 1), or use stereo input (indexes 0 and 1), or
    //use the channels 2 and 3 (the second input pair in a multichannel audio interface)

    int midiDeviceIndex; //setted when user choose MIDI as input method
    int channelIndex; //the group index (a group contain N LocalInputAudioNode instances)
public:
    LocalInputAudioNode(int parentChannelIndex, bool isMono=true);
    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    virtual int getSampleRate() const{return 0;}
    inline int getChannels() const{return audioInputRange.getChannels();}
    inline bool isMono() const{return audioInputRange.isMono();}
    inline bool isStereo() const{return audioInputRange.getChannels() == 2;}
    inline bool isNoInput() const{return audioInputRange.isEmpty();}
    inline bool isMidi() const{return midiDeviceIndex >= 0;}
    void setAudioInputSelection(int firstChannelIndex, int channelCount);
    void setMidiInputSelection(int midiDeviceIndex);
    void setToNoInput();
    inline int getMidiDeviceIndex() const{return midiDeviceIndex;}
    inline ChannelRange getAudioInputRange() const{return audioInputRange;}
    inline void setGlobalFirstInputIndex(int firstInputIndex){this->globalFirstInputIndex = firstInputIndex;}
    inline int getGroupChannelIndex() const {return channelIndex;}
    const Audio::SamplesBuffer& getLastBuffer() const{return internalBuffer;}
};
//++++++++++++++++++++++++

class LocalInputTestStreamer : public Audio::LocalInputAudioNode{//used to send a sine wave and test the audio transmission
public:
    LocalInputTestStreamer(float frequency, int sampleRate)
        :LocalInputAudioNode(0), osc(frequency, sampleRate) {
        osc.setGain(0.5);
    }

    void processReplacing(Audio::SamplesBuffer& in, Audio::SamplesBuffer& out){
        osc.processReplacing(in, out);//copy sine samples to out and simulate an input, just to test audio transmission
    }

private:
    Audio::OscillatorAudioNode osc;
};


}
