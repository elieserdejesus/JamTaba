#ifndef _LOCAL_INPUT_NODE_H_
#define _LOCAL_INPUT_NODE_H_

#include "AudioNode.h"

namespace Audio {

class LocalInputNode : public AudioNode
{
public:
    LocalInputNode(int parentChannelIndex, bool isMono = true);
    ~LocalInputNode();
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

}//namespace

#endif
