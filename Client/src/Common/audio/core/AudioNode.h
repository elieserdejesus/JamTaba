#ifndef AUDIO_NODE_H
#define AUDIO_NODE_H

#include <QSet>
#include <QMutex>
#include "SamplesBuffer.h"
#include "AudioDriver.h"
#include <QDebug>

namespace Midi   {
class MidiBuffer;
}

namespace Audio {
class AudioNodeProcessor : public QObject  // TODO - this inheritance is really necessary?
{
public:
    AudioNodeProcessor();
    virtual ~AudioNodeProcessor()
    {
    }

    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                         const Midi::MidiBuffer &midiBuffer) = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void updateGui() = 0;
    virtual void openEditor(const QPoint &centerOfScreen) = 0;
    virtual void closeEditor() = 0;
    virtual void setSampleRate(int newSampleRate)
    {
        Q_UNUSED(newSampleRate);
    }

    virtual void setBypass(bool state);
    bool isBypassed() const
    {
        return bypassed;
    }

    virtual bool isVirtualInstrument() const
    {
        return false;
    }

protected:
    bool bypassed;
};

// ++++++++++++++++++++++++++++++++++++++++++++
// this class is used to apply fade in and fade outs
class FaderProcessor : public AudioNodeProcessor
{
private:
    float currentGain;
    float startGain;
    float gainStep;
    int totalSamplesToProcess;
    int processedSamples;
public:
    FaderProcessor(float startGain, float endGain, int samplesToFade);
    void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                         const Midi::MidiBuffer &midiBuffer) override;
    bool finished();
    void reset();
    void resume() override
    {
    }

    void suspend() override
    {
    }
};
// ++++++++++++++++++++++++++++++++++++++++++++

class AudioNode : public QObject
{
    Q_OBJECT
public:
    AudioNode();
    virtual ~AudioNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiBuffer &midiBuffer);
    virtual void setMute(bool muted);
    void setSolo(bool soloed);
    inline bool isMuted() const
    {
        return muted;
    }

    inline bool isSoloed() const
    {
        return soloed;
    }

    virtual bool connect(AudioNode &other);
    virtual bool disconnect(AudioNode &otherNode);

    virtual void addProcessor(AudioNodeProcessor *newProcessor);
    void removeProcessor(AudioNodeProcessor *processor);
    void suspendProcessors();
    void resumeProcessors();
    virtual void updateProcessorsGui();

    void setGain(float gainValue);
    inline void setBoost(float boostValue)
    {
        this->boost = boostValue;
    }

    inline float getBoost() const
    {
        return boost;
    }

    inline float getGain() const
    {
        return gain;
    }

    void setPan(float pan);
    inline float getPan() const
    {
        return pan;
    }

    AudioPeak getLastPeak() const;

    inline void deactivate()
    {
        activated = false;
    }

    inline void activate()
    {
        activated = true;
    }

    virtual inline bool isActivated() const
    {
        return activated;
    }

    virtual void reset();// reset pan, gain, boost, etc

protected:

    int getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght);

    QSet<AudioNode *> connections;
    QList<AudioNodeProcessor *> processors;
    SamplesBuffer internalInputBuffer;
    SamplesBuffer internalOutputBuffer;

    mutable Audio::AudioPeak lastPeak;
    QMutex mutex; // used to protected connections manipulation because nodes can be added or removed by different threads
private:
    AudioNode(const AudioNode &other);
    AudioNode &operator=(const AudioNode &other);

    bool muted;
    bool soloed;

    bool activated;// used when room stream is played. All tracks are disabled, except the room streamer.

    float gain;
    float boost;

    // pan
    float pan;
    float leftGain;
    float rightGain;

    static const double ROOT_2_OVER_2;
    static const double PI_OVER_2;

    double resamplingCorrection;

    void updateGains();

signals:
    void gainChanged(float newGain);
    void panChanged(float newPan);
    // void boostChanged(float boostInDecibels);
    void muteChanged(bool muteStatus);
    void soloChanged(bool soloStatus);
};

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class OscillatorAudioNode : public AudioNode
{
public:
    OscillatorAudioNode(float frequency, int sampleRate);
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiBuffer &midiBuffer) override;
    virtual int getSampleRate() const
    {
        return sampleRate;
    }

private:
    float phase;
    const float phaseIncrement;
    int sampleRate;
};
// +++++++++++++++++

// ++++++++++++++++++
class LocalInputAudioNode : public AudioNode
{
public:
    LocalInputAudioNode(int parentChannelIndex, bool isMono = true);
    ~LocalInputAudioNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiBuffer &midiBuffer) override;
    virtual int getSampleRate() const
    {
        return 0;
    }

    inline int getChannels() const
    {
        return audioInputRange.getChannels();
    }

    bool isMono() const;

    bool isStereo() const;

    bool isNoInput() const;

    bool isMidi() const;

    bool isAudio() const;

    void setAudioInputSelection(int firstChannelIndex, int channelCount);

    void setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex);

    void setToNoInput();

    inline int getMidiDeviceIndex() const
    {
        return midiDeviceIndex;
    }

    inline int getMidiChannelIndex() const
    {
        return midiChannelIndex;
    }

    bool isReceivingAllMidiChannels() const;

    inline ChannelRange getAudioInputRange() const
    {
        return audioInputRange;
    }

    inline void setGlobalFirstInputIndex(int firstInputIndex)
    {
        this->globalFirstInputIndex = firstInputIndex;
    }

    inline int getGroupChannelIndex() const
    {
        return channelIndex;
    }

    const Audio::SamplesBuffer &getLastBuffer() const
    {
        return internalOutputBuffer;
    }

    void setProcessorsSampleRate(int newSampleRate);

    void closeProcessorsWindows();

    inline bool hasMidiActivity() const
    {
        return lastMidiActivity > 0;
    }

    quint8 getMidiActivityValue() const
    {
        return lastMidiActivity;
    }

    inline void resetMidiActivity()
    {
        lastMidiActivity = 0;
    }

    // overriding
    void addProcessor(AudioNodeProcessor *newProcessor) override;

    void reset();

    // local input tracks are always activated, so is possible play offline while listening to a room.
    // The other tracks (ninjam tracks) are deactivated when the 'room preview' is started.
    inline bool isActivated() const override
    {
        return true;
    }

private:
    int globalFirstInputIndex; // store the first input index selected globally by users in preferences menu

    ChannelRange audioInputRange;
    // store user selected input range. For example, user can choose just the
    // right input channel (index 1), or use stereo input (indexes 0 and 1), or
    // use the channels 2 and 3 (the second input pair in a multichannel audio interface)

    int midiDeviceIndex; // setted when user choose MIDI as input method
    int midiChannelIndex;

    int channelIndex; // the group index (a group contain N LocalInputAudioNode instances)

    enum InputMode {
        AUDIO, MIDI, DISABLED
    };

    InputMode inputMode = DISABLED;

    quint8 lastMidiActivity;// max velocity or control value
};
// ++++++++++++++++++++++++
class LocalInputGroup
{
public:
    LocalInputGroup(int groupIndex, Audio::LocalInputAudioNode *firstInput);
    ~LocalInputGroup();

    inline bool isEmpty() const
    {
        return groupedInputs.empty();
    }

    void addInput(Audio::LocalInputAudioNode *input);

    inline int getIndex() const
    {
        return groupIndex;
    }

    void mixGroupedInputs(Audio::SamplesBuffer &out);

    void removeInput(Audio::LocalInputAudioNode *input);

    int getMaxInputChannelsForEncoding() const;

    inline bool isTransmiting() const
    {
        return transmiting;
    }

    void setTransmitingStatus(bool transmiting);

private:
    int groupIndex;
    QList<Audio::LocalInputAudioNode *> groupedInputs;
    bool transmiting;
};
// ++++++++++++++++++++++++

class LocalInputTestStreamer : public Audio::LocalInputAudioNode // used to send a sine wave and test the audio transmission
{
public:
    LocalInputTestStreamer(float frequency, int sampleRate) :
        LocalInputAudioNode(0),
        osc(frequency, sampleRate)
    {
        osc.setGain(0.5);
    }

    void processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate,
                          const Midi::MidiBuffer &midiBuffer) override
    {
        osc.processReplacing(in, out, sampleRate, midiBuffer);// copy sine samples to out and simulate an input, just to test audio transmission
    }

private:
    Audio::OscillatorAudioNode osc;
};
}

#endif
