#include "LocalInputGroup.h"
#include "LocalInputNode.h"

using namespace Audio;

LocalInputGroup::LocalInputGroup(int groupIndex, Audio::LocalInputNode *firstInput) :
    groupIndex(groupIndex),
    transmiting(true)
{
    addInputNode(firstInput);
}

LocalInputGroup::~LocalInputGroup()
{
    groupedInputs.clear();
}

void LocalInputGroup::addInputNode(Audio::LocalInputNode *input)
{
    groupedInputs.append(input);
}

LocalInputNode *LocalInputGroup::getInputNode(quint8 index) const
{
    if (index < groupedInputs.size()) {
        return groupedInputs.at(index);
    }

    return nullptr;
}


void LocalInputGroup::mixGroupedInputs(Audio::SamplesBuffer &out)
{
    for (auto *inputTrack : groupedInputs) {
        if (!inputTrack->isMuted()) {
            auto lastBuffer = inputTrack->getLastBuffer();
            if (lastBuffer.getChannels() == out.getChannels()) {
                out.add(lastBuffer);
            }
            else {
                out.add(inputTrack->getLastBufferMixedToMono());
            }
        }
    }
}

void LocalInputGroup::removeInput(Audio::LocalInputNode *input)
{
    if (!groupedInputs.removeOne(input))
        qCritical() << "the input track was not removed!";
}

int LocalInputGroup::getMaxInputChannelsForEncoding() const
{
    if (groupedInputs.size() > 1)
        return 2;    // stereo encoding
    if (!groupedInputs.isEmpty()) {
        if (groupedInputs.first()->isMidi())
            return 2;    // just one midi track, use stereo encoding
        if (groupedInputs.first()->isAudio())
            return groupedInputs.first()->getAudioInputRange().getChannels();
        if (groupedInputs.first()->isNoInput())
            return 2;    // allow channels using noInput but processing some vst looper in stereo
    }
    return 0;    // no channels to encoding
}

void LocalInputGroup::setTransmitingStatus(bool transmiting)
{
    this->transmiting = transmiting;
}
