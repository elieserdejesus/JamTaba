#ifndef METRONOMETRACKNODE_H
#define METRONOMETRACKNODE_H

#include "core/AudioNode.h"

namespace Audio {
class SamplesBuffer;


class MetronomeTrackNode : public Audio::AudioNode
{
public:
    MetronomeTrackNode(QString metronomeWaveFile, int localSampleRate);

    ~MetronomeTrackNode();
    virtual void processReplacing(const SamplesBuffer&in, SamplesBuffer& out, int SampleRate, const Midi::MidiBuffer& midiBuffer);
    void setSamplesPerBeat(long samplesPerBeat);
    void setIntervalPosition(long intervalPosition);
    void resetInterval();

    void setBeatsPerAccent(int beatsPerAccent) ;//pass zero to turn off accents

    inline bool isPlayingAccents() const{ return beatsPerAccent > 0; }

    virtual int getSampleRate() const{return waveFileSampleRate;}
    inline int getBeatsPerAccent() const{return beatsPerAccent;}
private:
    SamplesBuffer* clickSoundBuffer;
    SamplesBuffer* firstIntervalBeatBuffer;
    SamplesBuffer* firstMeasureBeatBuffer;

    long samplesPerBeat;
    long intervalPosition;
    long beatPosition;//controla a amostra atual, vai incrementando conforme as amostras são tocadas
    int currentBeat;
    int beatsPerAccent;

    quint32 waveFileSampleRate;//metrono wav file sample rate
    SamplesBuffer* readWavFile(QString fileName, quint32 &sampleRate);
    SamplesBuffer* getBuffer(int beat);//return the correct buffer to play in each beat
};

}

#endif // METRONOMETRACKNODE_H
