#ifndef METRONOMETRACKNODE_H
#define METRONOMETRACKNODE_H

#include "core/AudioNode.h"

namespace Audio {

class SamplesBuffer;

class MetronomeTrackNode : public Audio::AudioNode
{

public:
    MetronomeTrackNode(const Audio::SamplesBuffer &firstBeatSamples, const Audio::SamplesBuffer &secondaryBeatSamples);

    ~MetronomeTrackNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int SampleRate, std::vector<Midi::MidiMessage> &midiBuffer) override;
    void setSamplesPerBeat(long samplesPerBeat);
    void setIntervalPosition(long intervalPosition);
    void resetInterval();

    void setBeatsPerAccent(int beatsPerAccent); // pass zero to turn off accents

    bool isPlayingAccents() const;

    int getBeatsPerAccent() const;

    void setPrimaryBeatSamples(const Audio::SamplesBuffer &firstBeatSamples);
    void setSecondaryBeatSamples(const Audio::SamplesBuffer &secondaryBeatSamples);

private:
    SamplesBuffer secondaryBeatBuffer;
    SamplesBuffer firstBeatBuffer;

    long samplesPerBeat;
    long intervalPosition;
    long beatPosition;
    int currentBeat;
    int beatsPerAccent;

    SamplesBuffer *getSamplesBuffer(int beat); // return the correct buffer to play in each beat
};

inline bool MetronomeTrackNode::isPlayingAccents() const
{
    return beatsPerAccent > 0;
}

inline int MetronomeTrackNode::getBeatsPerAccent() const
{
    return beatsPerAccent;
}

} // namespace

#endif // METRONOMETRACKNODE_H
