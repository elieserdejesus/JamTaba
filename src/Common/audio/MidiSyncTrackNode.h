#ifndef MIDISYNCTRACKNODE_H
#define MIDISYNCTRACKNODE_H

#include "core/AudioNode.h"

namespace controller{
    class MainController;
}

namespace audio {

using controller::MainController;

class MidiSyncTrackNode : public audio::AudioNode
{

public:
    MidiSyncTrackNode(MainController *controller);

    ~MidiSyncTrackNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<midi::MidiMessage> &midiBuffer) override;
    void setSamplesPerPulse(long samplesPerPulse);
    void setIntervalPosition(long intervalPosition);
    void resetInterval();

private:
    long samplesPerPulse;
    long intervalPosition;
    long pulsePosition;
    int currentPulse;
    int lastPlayedPulse;

    MainController *mainController;
};

} // namespace

#endif // MIDISYNCTRACKNODE_H
