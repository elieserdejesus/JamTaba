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

    void setBeatsPerAccent(int beatsPerAccent); // pass zero to turn off accents
    void setAccentBeats(QList<int> accents); // pass empty array or null to turn off accents

    bool isPlayingAccents() const;

    int getBeatsPerAccent() const; // will return zero even if isPlayingAccents() when pattern is uneven

    QList<int> getAccentBeats(); // will return zero even if isPlayingAccents() when pattern is uneven

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

inline QList<int> MetronomeTrackNode::getAccentBeats()
{
    return this->accentBeats;
}

} // namespace

#endif // METRONOMETRACKNODE_H
