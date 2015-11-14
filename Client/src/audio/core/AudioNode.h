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

class AudioNodeProcessor : public QObject{
public:
    AudioNodeProcessor();
    virtual ~AudioNodeProcessor(){}
    virtual void process(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, const Midi::MidiBuffer& midiBuffer) = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void updateGui() = 0;
    virtual void openEditor(QPoint centerOfScreen) = 0;
    virtual void closeEditor() = 0;
    virtual void setSampleRate(int newSampleRate){ Q_UNUSED(newSampleRate);}
    virtual void setBypass(bool state);
    bool isBypassed() const{return bypassed;}
    virtual bool isVirtualInstrument() const{ return false;}

protected:
    bool bypassed;
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
    virtual void process(const Audio::SamplesBuffer& in, Audio::SamplesBuffer &out, const Midi::MidiBuffer& midiBuffer);
    bool finished();
    void reset();
    void resume(){}
    void suspend(){}
};
//++++++++++++++++++++++++++++++++++++++++++++



class AudioNode {

public:
    AudioNode();
    virtual ~AudioNode();

    virtual void processReplacing(const SamplesBuffer& in, SamplesBuffer& out, int sampleRate, const Midi::MidiBuffer& midiBuffer);
    virtual inline void setMuteStatus(bool muted){ this->muted = muted;}
    void inline setSoloStatus(bool soloed){ this->soloed = soloed; }
    inline bool isMuted() const {return muted;}
    inline bool isSoloed() const {return soloed;}

    virtual bool connect(AudioNode &other) ;
    virtual bool disconnect(AudioNode &otherNode);

    virtual void addProcessor(AudioNodeProcessor *newProcessor);
    void removeProcessor(AudioNodeProcessor* processor);
    void suspendProcessors();
    void resumeProcessors();
    virtual void updateProcessorsGui();

    inline void setGain(float gainValue){this->gain = gainValue;}
    inline void setBoost(float boostValue){this->boost = boostValue;}

    inline float getBoost() const {return boost;}

    inline float getGain() const{return gain;}

    void setPan(float pan);
    inline float getPan() const {return pan;}

    AudioPeak getLastPeak() const;

    inline void deactivate(){activated = false;}
    inline void activate(){activated = true;}
    inline bool isActivated() const{return activated;}

protected:

    int getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght) ;

    QSet<AudioNode*> connections;
    QList<AudioNodeProcessor*> processors;
    SamplesBuffer internalInputBuffer;
    SamplesBuffer internalOutputBuffer;

    mutable Audio::AudioPeak lastPeak;
    QMutex mutex; //used to protected connections manipulation because nodes can be added or removed by different threads
private:
    AudioNode(const AudioNode& other);
    AudioNode& operator=(const AudioNode& other);

    bool muted;
    bool soloed;

    bool activated;//used when room stream is played. All tracks are disabled, except the room streamer.

    float gain;
    float boost;

    //pan
    float pan;
    float leftGain;
    float rightGain;

    static const double ROOT_2_OVER_2;// = 1.414213562373095;// *0.5;
    static const double PI_OVER_2;// = 3.141592653589793238463 * 0.5;

    double resamplingCorrection;

    void updateGains();
};

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class OscillatorAudioNode : public AudioNode{

public:
    OscillatorAudioNode(float frequency, int sampleRate);
    virtual void processReplacing(const SamplesBuffer&in, SamplesBuffer& out, int sampleRate, const Midi::MidiBuffer &midiBuffer);
    virtual int getSampleRate() const{return sampleRate;}
private:
    float phase;
    const float phaseIncrement;
    int sampleRate;
};
//+++++++++++++++++

//++++++++++++++++++
class LocalInputAudioNode : public AudioNode{

public:
    LocalInputAudioNode(int parentChannelIndex, bool isMono=true);
    ~LocalInputAudioNode();
    virtual void processReplacing(const SamplesBuffer& in, SamplesBuffer& out, int sampleRate, const Midi::MidiBuffer &midiBuffer);
    virtual int getSampleRate() const{return 0;}
    inline int getChannels() const{return audioInputRange.getChannels();}
    bool isMono() const;//{return audioInputRange.isMono();}
    bool isStereo() const;//{return audioInputRange.getChannels() == 2;}
    bool isNoInput() const;//{return audioInputRange.isEmpty();}
    bool isMidi() const;//{return midiDeviceIndex >= 0;}
    bool isAudio() const;
    void setAudioInputSelection(int firstChannelIndex, int channelCount);
    void setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex);
    void setToNoInput();
    inline int getMidiDeviceIndex() const{return midiDeviceIndex;}
    inline int getMidiChannelIndex() const{return midiChannelIndex;}
    bool isReceivingAllMidiChannels() const;
    inline ChannelRange getAudioInputRange() const{return audioInputRange;}
    inline void setGlobalFirstInputIndex(int firstInputIndex){this->globalFirstInputIndex = firstInputIndex;}
    inline int getGroupChannelIndex() const {return channelIndex;}
    const Audio::SamplesBuffer& getLastBuffer() const{return internalOutputBuffer;}
    void setProcessorsSampleRate(int newSampleRate);
    void closeProcessorsWindows();

    //overriding
    void addProcessor( AudioNodeProcessor* newProcessor);
private:
    int globalFirstInputIndex; //store the first input index selected globally by users in preferences menu
    ChannelRange audioInputRange;
    //store user selected input range. For example, user can choose just the
    //right input channel (index 1), or use stereo input (indexes 0 and 1), or
    //use the channels 2 and 3 (the second input pair in a multichannel audio interface)

    int midiDeviceIndex; //setted when user choose MIDI as input method
    int midiChannelIndex;


    int channelIndex; //the group index (a group contain N LocalInputAudioNode instances)

    enum InputMode{ AUDIO, MIDI, DISABLED };

    InputMode inputMode = DISABLED;



};
//++++++++++++++++++++++++

class LocalInputTestStreamer : public Audio::LocalInputAudioNode{//used to send a sine wave and test the audio transmission
public:
    LocalInputTestStreamer(float frequency, int sampleRate)
        :LocalInputAudioNode(0), osc(frequency, sampleRate) {
        osc.setGain(0.5);
    }

    void processReplacing(const Audio::SamplesBuffer& in, Audio::SamplesBuffer& out, int sampleRate, const Midi::MidiBuffer& midiBuffer){
        osc.processReplacing(in, out, sampleRate, midiBuffer);//copy sine samples to out and simulate an input, just to test audio transmission
    }

private:
    Audio::OscillatorAudioNode osc;
};


}
