#ifndef METRONOMETRACKNODE_H
#define METRONOMETRACKNODE_H

#include "core/AudioNode.h"

namespace Audio {

class SamplesBuffer;

class MetronomeTrackNode : public Audio::AudioNode
{

public:
    MetronomeTrackNode(const Audio::SamplesBuffer &firstBeatSamples, const Audio::SamplesBuffer &offBeatSamples, const SamplesBuffer &accentBeatSamples);

    ~MetronomeTrackNode();
    void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int SampleRate, std::vector<Midi::MidiMessage> &midiBuffer) override;
    void setSamplesPerBeat(long samplesPerBeat);
    void setIntervalPosition(long intervalPosition);
    void resetInterval();

    void setBeatsPerAccent(int beatsPerAccent, int currentBpi); // pass zero to turn off accents

    bool isPlayingAccents() const;

    int getBeatsPerAccent() const; // will return zero even if isPlayingAccents() when pattern is uneven

    void setAccentBeats(QList<int> accents); // pass empty list to turn off accents
    QList<int> getAccentBeats(); // returns the beats with accents

    void setPrimaryBeatSamples(const Audio::SamplesBuffer &firstBeatSamples);
    void setOffBeatSamples(const Audio::SamplesBuffer &offBeatSamples);
    void setAccentBeatSamples(const Audio::SamplesBuffer &accentBeatSamples);

private:
    SamplesBuffer firstBeatBuffer;
    SamplesBuffer offBeatBuffer;
    SamplesBuffer accentBeatBuffer;

    long samplesPerBeat;
    long intervalPosition;
    long beatPosition;
    int currentBeat;
    QList<int> accentBeats = QList<int>();

    SamplesBuffer *getSamplesBuffer(int beat); // return the correct buffer to play in each beat
};

inline bool MetronomeTrackNode::isPlayingAccents() const
{
    return accentBeats.length() > 0;
}

} // namespace

#endif // METRONOMETRACKNODE_H
