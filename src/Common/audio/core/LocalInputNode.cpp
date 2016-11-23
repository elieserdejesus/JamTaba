#include "LocalInputNode.h"
#include "audio/core/AudioNodeProcessor.h"
#include "midi/MidiMessageBuffer.h"
#include "midi/MidiMessage.h"
#include "MainController.h"

using namespace Audio;

LocalInputNode::LocalInputNode(Controller::MainController *mainController, int parentChannelIndex, bool isMono) :
    channelIndex(parentChannelIndex),
    lastMidiActivity(0),
    midiChannelIndex(-1),
    midiDeviceIndex(-1),
    midiLowerNote(0),
    midiHigherNote(127),
    transpose(0),
    learningMidiNote(false),
    mainController(mainController),
    stereoInverted(false)
{
    Q_UNUSED(isMono)
    setToNoInput();
}

LocalInputNode::~LocalInputNode()
{
}

bool LocalInputNode::isMono() const
{
    return isAudio() && audioInputRange.isMono();
}

bool LocalInputNode::isStereo() const
{
    return isAudio() && audioInputRange.getChannels() == 2;
}

bool LocalInputNode::isNoInput() const
{
    return inputMode == DISABLED;
}

bool LocalInputNode::isMidi() const
{
    return inputMode == MIDI;// && midiDeviceIndex >= 0;
}

bool LocalInputNode::isAudio() const
{
    return inputMode == AUDIO;
}

void LocalInputNode::setProcessorsSampleRate(int newSampleRate)
{
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i])
            processors[i]->setSampleRate(newSampleRate);
    }
}

void LocalInputNode::closeProcessorsWindows()
{
    for (int i = 0; i < MAX_PROCESSORS_PER_TRACK; ++i) {
        if (processors[i])
            processors[i]->closeEditor();
    }
}

void LocalInputNode::addProcessor(AudioNodeProcessor *newProcessor, quint32 slotIndex)
{
    AudioNode::addProcessor(newProcessor, slotIndex);

    // if newProcessor is the first added processor and is a virtual instrument (VSTi) change the input selection to midi
    if (slotIndex == 0 && newProcessor->isVirtualInstrument()) {
        if (!isMidi())
            setMidiInputSelection(0, -1);// select the first midi device, all channels (-1)
    }
}

void LocalInputNode::setAudioInputSelection(int firstChannelIndex, int channelCount)
{
    audioInputRange = ChannelRange(firstChannelIndex, channelCount);
    if (audioInputRange.isMono())
        internalInputBuffer.setToMono();
    else
        internalInputBuffer.setToStereo();

    midiDeviceIndex = -1;// disable midi input
    inputMode = AUDIO;
}

void LocalInputNode::setToNoInput()
{
    audioInputRange = ChannelRange(-1, 0);// disable audio input
    midiDeviceIndex = -1;// disable midi input
    inputMode = DISABLED;
}

void LocalInputNode::setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex)
{
    audioInputRange = ChannelRange(-1, 0);// disable audio input
    this->midiDeviceIndex = midiDeviceIndex;
    this->midiChannelIndex = midiChannelIndex;
    inputMode = MIDI;
}

void LocalInputNode::setMidiHigherNote(quint8 newHigherNote)
{
    if (newHigherNote > 127)
        newHigherNote = 127;
    midiHigherNote = newHigherNote;
    if (midiHigherNote < midiLowerNote)
        midiLowerNote = midiHigherNote;
}

void LocalInputNode::setMidiLowerNote(quint8 newLowerNote)
{
    if (newLowerNote > 127)
        newLowerNote = 127;

    midiLowerNote = newLowerNote;
    if (midiLowerNote > midiHigherNote)
        midiHigherNote = midiLowerNote;
}

bool LocalInputNode::isReceivingAllMidiChannels() const
{
    if (inputMode == MIDI)
        return midiChannelIndex < 0 || midiChannelIndex > 16;
    return false;
}

void LocalInputNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                           int sampleRate, const Midi::MidiMessageBuffer &midiBuffer)
{
    Q_UNUSED(sampleRate);

    /* The input buffer (in) is a multichannel buffer. So, this buffer contains
     * all channels grabbed from soundcard inputs. If the user select a range of 4
     * input channels in audio preferences this buffer will contain 4 channels.
     *
     * A LocalInputAudioNode instance grab only your input range from this input SamplesBuffer.
     * Other LocalInputAudioNode instances will read other channels from input SamplesBuffer.
     */

    Midi::MidiMessageBuffer filteredMidiBuffer(midiBuffer.getMessagesCount());
    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();
    internalOutputBuffer.zero();

    if (!isNoInput()) {
        if (isAudio()) {// using audio input
            if (audioInputRange.isEmpty())
                return;
            internalInputBuffer.set(in, audioInputRange.getFirstChannel(), audioInputRange.getChannels());
        } else if (isMidi()) {// just in case
            int messagesCount = midiBuffer.getMessagesCount();
            for (int m = 0; m < messagesCount; ++m) {
                Midi::MidiMessage message = midiBuffer.getMessage(m);
                if (canAcceptMidiMessage(message)) {

                    if (message.isNote() && transpose != 0)
                        message.transpose(transpose);

                    filteredMidiBuffer.addMessage(message);

                    // save the midi activity peak value for notes or controls
                    if (message.isNoteOn() || message.isControl()) {
                        quint8 activityValue = message.getData2();
                        if (activityValue > lastMidiActivity)
                            lastMidiActivity = activityValue;
                    }
                }
            }
        }
    }

    AudioNode::processReplacing(in, out, sampleRate, filteredMidiBuffer);
}

void LocalInputNode::preFaderProcess(SamplesBuffer &out) // this function is called by the base class AudioNode when processing audio. It's the TemplateMethod design pattern idea.
{
    if (stereoInverted)
        out.invertStereo();
}

void LocalInputNode::setStereoInversion(bool inverted)
{
    if (isMono())
        return;

    if (this->stereoInverted != inverted)
    {
        this->stereoInverted = inverted;
        emit stereoInversionChanged(inverted);
    }
}

bool LocalInputNode::isStereoInverted() const
{
    return !isMono() && stereoInverted;
}

void LocalInputNode::setTranspose(qint8 transpose)
{
    if ( qAbs(transpose) <= 24) {
        this->transpose = transpose;
    }
}

bool LocalInputNode::canAcceptMidiMessage(const Midi::MidiMessage &message) const
{
    bool canAcceptDevice = message.getSourceID() == midiDeviceIndex;
    bool canAcceptChannel = isReceivingAllMidiChannels() || message.getChannel() == midiChannelIndex;
    bool canAcceptRange = true;

    if (message.isNote()) {
        int midiNote = message.getData1();
        if (!learningMidiNote) {//check midi range if not learning
            canAcceptRange = midiNote >= midiLowerNote && midiNote <= midiHigherNote;
        }
        else{ //is learning midi notes
            emit midiNoteLearned((quint8)midiNote);
            return false; //when learning all messages are bypassed
        }
    }
    return (canAcceptDevice && canAcceptChannel && canAcceptRange);
}

QList<Midi::MidiMessage> LocalInputNode::pullMidiMessagesGeneratedByPlugins() const
{
    return mainController->pullMidiMessagesFromPlugins().toList();
}

void LocalInputNode::startMidiNoteLearn()
{
    this->learningMidiNote = true;
}

void LocalInputNode::stopMidiNoteLearn()
{
    this->learningMidiNote = false;
}
// ++++++++++++=
void LocalInputNode::reset()
{
    AudioNode::reset();
    setToNoInput();

    stereoInverted = false;
    emit stereoInversionChanged(stereoInverted);
}
