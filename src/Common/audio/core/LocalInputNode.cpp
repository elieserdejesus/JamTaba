#include "LocalInputNode.h"
#include "audio/core/AudioNodeProcessor.h"
#include "midi/MidiMessage.h"
#include "MainController.h"

using namespace Audio;

LocalInputNode::MidiInput::MidiInput()
    : lastMidiActivity(0),
      channel(-1),
      device(-1),
      lowerNote(0),
      higherNote(127),
      transpose(0),
      learning(false)
{
    //
}

void LocalInputNode::MidiInput::disable()
{
    device = -1;
}

void LocalInputNode::MidiInput::setHigherNote(quint8 note)
{
    higherNote = qMin(note, (quint8)127);

    if (higherNote < lowerNote)
        lowerNote = higherNote;
}

void LocalInputNode::MidiInput::setLowerNote(quint8 note)
{
    lowerNote = qMin(note, (quint8)127);

    if (lowerNote > higherNote)
        higherNote = lowerNote;
}

bool LocalInputNode::MidiInput::isReceivingAllMidiChannels() const
{
    return channel < 0 || channel > 16;
}

void LocalInputNode::MidiInput::updateActivity(const Midi::MidiMessage &message)
{
    if (message.isNoteOn() || message.isControl()) {
        quint8 activityValue = message.getData2();
        if (activityValue > lastMidiActivity)
            lastMidiActivity = activityValue;
    }
}

void LocalInputNode::MidiInput::setTranspose(quint8 newTranspose)
{
    if ( qAbs(newTranspose) <= 24) {
        this->transpose = newTranspose;
    }
}

bool LocalInputNode::MidiInput::accept(const Midi::MidiMessage &message) const
{
    bool canAcceptDevice = message.getSourceDeviceIndex() == device;
    bool canAcceptChannel = isReceivingAllMidiChannels() || message.getChannel() == channel;
    bool canAcceptRange = true;

    if (message.isNote()) {
        int midiNote = message.getData1();
        canAcceptRange = midiNote >= lowerNote && midiNote <= higherNote;
    }

    return (canAcceptDevice && canAcceptChannel && canAcceptRange);
}

// ---------------------------------------------------------------------

LocalInputNode::LocalInputNode(Controller::MainController *mainController, int parentChannelIndex, bool isMono) :
    channelGroupIndex(parentChannelIndex),
    mainController(mainController),
    stereoInverted(false),
    receivingRoutedMidiInput(false),
    routingMidiInput(false)
{
    Q_UNUSED(isMono)
    setToNoInput();
}

LocalInputNode::~LocalInputNode()
{
    //
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

    midiInput.disable();
    inputMode = AUDIO;
}

void LocalInputNode::setToNoInput()
{
    audioInputRange = ChannelRange(-1, 0);// disable audio input
    midiInput.disable();
    inputMode = DISABLED;
}

void LocalInputNode::setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex)
{
    audioInputRange   = ChannelRange(-1, 0);// disable audio input
    midiInput.device  = midiDeviceIndex;
    midiInput.channel = midiChannelIndex;
    inputMode = MIDI;
}

void LocalInputNode::setMidiHigherNote(quint8 newHigherNote)
{
    midiInput.setHigherNote(newHigherNote);
}

void LocalInputNode::setMidiLowerNote(quint8 newLowerNote)
{
    midiInput.setLowerNote(newLowerNote);
}

bool LocalInputNode::isReceivingAllMidiChannels() const
{
    if (inputMode == MIDI)
        return midiInput.isReceivingAllMidiChannels();
    return false;
}

void LocalInputNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out,
                                           int sampleRate, std::vector<Midi::MidiMessage> &midiBuffer)
{
    Q_UNUSED(sampleRate);

    /* The input buffer (in) is a multichannel buffer. So, this buffer contains
     * all channels grabbed from soundcard inputs. If the user select a range of 4
     * input channels in audio preferences this buffer will contain 4 channels.
     *
     * A LocalInputAudioNode instance grab only your input range from this input SamplesBuffer.
     * Other LocalInputAudioNode instances will read other channels from input SamplesBuffer.
     */

    std::vector<Midi::MidiMessage> filteredMidiBuffer(midiBuffer.size());
    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();
    internalOutputBuffer.zero();

    if (!isNoInput()) {
        if (isAudio()) {// using audio input
            if (audioInputRange.isEmpty())
                return;

            internalInputBuffer.set(in, audioInputRange.getFirstChannel(), audioInputRange.getChannels());
        }
        else if (isMidi() && !midiBuffer.empty()) {
            processIncommingMidi(midiBuffer, filteredMidiBuffer);
        }
    }

    if (receivingRoutedMidiInput && !midiBuffer.empty()) { // vocoders, for example, can receive midi input from second subchannel
        quint8 subchannelIndex = 1;// second subchannel
        LocalInputNode *secondSubchannel = mainController->getInputTrackInGroup(channelGroupIndex, subchannelIndex);
        if (secondSubchannel && secondSubchannel->isMidi()) {
            secondSubchannel->processIncommingMidi(midiBuffer, filteredMidiBuffer);
        }
    }

    if (isRoutingMidiInput()) {
        lastPeak.update(AudioPeak()); // ensure the audio meters will be ZERO

        return; // when routing midi this track will not render midi data, this data will be rendered by first subchannel. But the midi data is processed above to update MIDI activity meter
    }

    AudioNode::processReplacing(in, out, sampleRate, filteredMidiBuffer); // only the filtered midi messages are sended to rendering code
}

void LocalInputNode::setRoutingMidiInput(bool routeMidiInput)
{
    quint8 subchannelIndex = 0; // first subchannel
    LocalInputNode *firstSubchannel = mainController->getInputTrackInGroup(channelGroupIndex, subchannelIndex);

    if (firstSubchannel == this)
        return; // midi routing is not allowed in first subchannel

    if (firstSubchannel) {
        routingMidiInput = isMidi() && routeMidiInput;

        if (routingMidiInput)
            receivingRoutedMidiInput = false;

        firstSubchannel->setReceivingRoutedMidiInput(routingMidiInput);
    }
    else {
        qCritical() << "First subchannel is null!";
    }
}

void LocalInputNode::setReceivingRoutedMidiInput(bool receiveRoutedMidiInput)
{
    receivingRoutedMidiInput = receiveRoutedMidiInput;

    if (receivingRoutedMidiInput)
        routingMidiInput = false;
}

void LocalInputNode::processIncommingMidi(std::vector<Midi::MidiMessage> &inBuffer, std::vector<Midi::MidiMessage> &outBuffer)
{
    std::vector<Midi::MidiMessage>::iterator iterator = inBuffer.begin();
    while(iterator != inBuffer.end()) {
        Midi::MidiMessage message(*iterator);
        if (canProcessMidiMessage(message)) {
            message.transpose(getTranspose());

            outBuffer.push_back(message);

            // save the midi activity peak value for notes or controls
            midiInput.updateActivity(message);

            iterator = inBuffer.erase(iterator);
        }
        else {
            ++iterator;
        }
    }
}

qint8 LocalInputNode::getTranspose() const
{
    if (!receivingRoutedMidiInput) {
        return midiInput.transpose;
    }
    else {
        quint8 subchannelIndex = 1; //second subchannel
        LocalInputNode *secondSubchannel = mainController->getInputTrackInGroup(channelGroupIndex, subchannelIndex);
        if (secondSubchannel && secondSubchannel->isMidi()) {
            return secondSubchannel->midiInput.transpose;
        }
    }

    return 0;
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
    midiInput.setTranspose(transpose);
}

bool LocalInputNode::canProcessMidiMessage(const Midi::MidiMessage &message) const
{
    if (midiInput.isLearning()) {
        quint8 midiNote = (quint8)message.getData1();
        emit midiNoteLearned(midiNote);

        return false; //when learning all messages are bypassed
    }


    return midiInput.accept(message);

}

std::vector<Midi::MidiMessage> LocalInputNode::pullMidiMessagesGeneratedByPlugins() const
{
    return mainController->pullMidiMessagesFromPlugins();
}

void LocalInputNode::startMidiNoteLearn()
{
    midiInput.learning = true;
}

void LocalInputNode::stopMidiNoteLearn()
{
    midiInput.learning = false;
}
// ++++++++++++=
void LocalInputNode::reset()
{
    AudioNode::reset();
    setToNoInput();

    stereoInverted = false;

    emit stereoInversionChanged(stereoInverted);
}
