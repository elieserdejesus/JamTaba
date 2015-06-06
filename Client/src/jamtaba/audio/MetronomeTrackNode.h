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
    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    void setSamplesPerBeat(long samplesPerBeat);
    void setIntervalPosition(long intervalPosition);
    void reset();
private:
    SamplesBuffer* clickSoundBuffer;
    SamplesBuffer* firstIntervalBeatBuffer;
    SamplesBuffer* firstMeasureBeatBuffer;

    //unsigned int clickBufferOffset;

    long samplesPerBeat;
    long intervalPosition;
    long beatPosition;//controla a amostra atual, vai incrementando conforme as amostras são tocadas

    SamplesBuffer* readWavFile(QString fileName, quint32 &sampleRate);//the second parameter is a output parameter
};

}

#endif // METRONOMETRACKNODE_H
