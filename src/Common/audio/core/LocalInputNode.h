#ifndef _LOCAL_INPUT_NODE_H_
#define _LOCAL_INPUT_NODE_H_

#include "AudioNode.h"
#include "../AudioLooper.h"

namespace Midi {
    class MidiMessage;
}

namespace Controller {
class MainController;
}

namespace Audio {

class LocalInputNode : public AudioNode
{
    Q_OBJECT

public:
    LocalInputNode(Controller::MainController *mainController, int parentChannelIndex, bool isMono = true);
    ~LocalInputNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<Midi::MidiMessage> &midiBuffer) override;
    virtual int getSampleRate() const;

    int getChannels() const;

    bool isMono() const;

    bool isStereo() const;

    bool isNoInput() const;

    bool isMidi() const;

    bool isAudio() const;

    void setStereoInversion(bool stereoInverted);

    bool isStereoInverted() const;

    void setAudioInputSelection(int firstChannelIndex, int channelCount);

    void setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex);

    void setToNoInput();

    int getMidiDeviceIndex() const;

    int getMidiChannelIndex() const;

    bool isReceivingAllMidiChannels() const;

    std::vector<Midi::MidiMessage> pullMidiMessagesGeneratedByPlugins() const override;

    ChannelRange getAudioInputRange() const;

    int getChanneGrouplIndex() const;

    const Audio::SamplesBuffer &getLastBuffer() const;

    void setProcessorsSampleRate(int newSampleRate);

    void closeProcessorsWindows();

    bool hasMidiActivity() const;

    quint8 getMidiActivityValue() const;

    void resetMidiActivity();

    void setMidiLowerNote(quint8 newLowerNote);

    void setMidiHigherNote(quint8 newHigherNote);

    void setTranspose(qint8 transpose);

    qint8 getTranspose() const;

    quint8 getMidiLowerNote() const;

    quint8 getMidiHigherNote() const;

    void startMidiNoteLearn();
    void stopMidiNoteLearn();
    bool isLearningMidiNote() const;

    void reset() override;

    /** local input tracks are always activated, so is possible play offline while listening to a room.
     The other tracks (ninjam tracks) are deactivated when the 'room preview' is started. Deactivated tracks are not rendered. */
    bool isActivated() const override;

    bool isReceivingRoutedMidiInput() const;
    void setReceivingRoutedMidiInput(bool receiveRoutedMidiInput);
    bool isRoutingMidiInput() const;
    void setRoutingMidiInput(bool routeMidiInput);

    void startNewLoopCycle(uint intervalLenght);
    void stopLooper();

    Audio::Looper *getLooper() const;

signals:
    void midiNoteLearned(quint8 midiNote) const;
    void stereoInversionChanged(bool stereoInverted);

protected:
    void preFaderProcess(Audio::SamplesBuffer &out) override;

private:
    //int globalFirstInputIndex; // store the first input index selected globally by users in preferences menu

    ChannelRange audioInputRange;
    // store user selected input range. For example, user can choose just the
    // right input channel (index 1), or use stereo input (indexes 0 and 1), or
    // use the channels 2 and 3 (the second input pair in a multichannel audio interface)

    class MidiInput
    {
    public:
        MidiInput();
        void disable();
        void setHigherNote(quint8 higherNote);
        void setLowerNote(quint8 lowerNote);
        bool isReceivingAllMidiChannels() const;
        void updateActivity(const Midi::MidiMessage &message);
        bool accept(const Midi::MidiMessage &message) const;
        void setTranspose(quint8 newTranspose);
        inline bool isLearning() const { return learning; }

        int device; // setted when user choose MIDI as input method
        int channel;
        quint8 lastMidiActivity;// last max velocity or control value
        quint8 lowerNote;
        quint8 higherNote;
        qint8 transpose;
        bool learning; //is waiting to learn a midi note?

    };

    MidiInput midiInput;

    quint8 getTransposeAmmount() const;

    int channelGroupIndex; // the group index (a group contain N LocalInputNode instances)

    bool stereoInverted;

    bool receivingRoutedMidiInput; // true when this is the first subchannel and is receiving midi input from second subchannel (rounted midi input)? issue #102
    bool routingMidiInput; // true when this is the second channel and is sending midi messages to the first channel

    Controller::MainController *mainController;

    enum InputMode {
        AUDIO, MIDI, DISABLED
    };

    InputMode inputMode = DISABLED;

    bool canProcessMidiMessage(const Midi::MidiMessage &msg) const;

    void processIncommingMidi(std::vector<Midi::MidiMessage> &inBuffer, std::vector<Midi::MidiMessage> &outBuffer);

    Audio::Looper* looper;

};

inline Audio::Looper *LocalInputNode::getLooper() const
{
    return looper;
}

inline bool LocalInputNode::isRoutingMidiInput() const
{
    return isMidi() && routingMidiInput;
}

inline bool LocalInputNode::isReceivingRoutedMidiInput() const
{
    return receivingRoutedMidiInput;
}

inline bool LocalInputNode::isLearningMidiNote() const
{
    return midiInput.learning;
}

inline quint8 LocalInputNode::getMidiHigherNote() const
{
    return midiInput.higherNote;
}

inline quint8 LocalInputNode::getMidiLowerNote() const
{
    return midiInput.lowerNote;
}

inline int LocalInputNode::getSampleRate() const
{
    return 0;
}

inline int LocalInputNode::getChannels() const
{
    return audioInputRange.getChannels();
}

inline int LocalInputNode::getMidiDeviceIndex() const
{
    return midiInput.device;
}

inline int LocalInputNode::getMidiChannelIndex() const
{
    return midiInput.channel;
}

inline ChannelRange LocalInputNode::getAudioInputRange() const
{
    return audioInputRange;
}

inline int LocalInputNode::getChanneGrouplIndex() const
{
    return channelGroupIndex;
}

inline const Audio::SamplesBuffer &LocalInputNode::getLastBuffer() const
{
    return internalOutputBuffer;
}

inline bool LocalInputNode::hasMidiActivity() const
{
    return midiInput.lastMidiActivity > 0;
}

inline quint8 LocalInputNode::getMidiActivityValue() const
{
    return midiInput.lastMidiActivity;
}

inline void LocalInputNode::resetMidiActivity()
{
    midiInput.lastMidiActivity = 0;
}

inline bool LocalInputNode::isActivated() const
{
    return true;
}

}//namespace

#endif
