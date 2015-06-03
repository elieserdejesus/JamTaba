#ifndef METRONOMETRACKNODE_H
#define METRONOMETRACKNODE_H

#include "core/AudioNode.h"

namespace Audio {
class SamplesBuffer;


class MetronomeTrackNode : public Audio::AudioNode
{
public:
    MetronomeTrackNode(QString metronomeWaveFile);
    ~MetronomeTrackNode();
    virtual void processReplacing(SamplesBuffer&in, SamplesBuffer& out);
    void setSamplesPerBeat(long samplesPerBeat);
private:
    SamplesBuffer* clickSoundBuffer;
    unsigned int clickBufferOffset;

    long samplesPerBeat;
    long beatSample;//controla a amostra atual, vai incrementando conforme as amostras são tocadas

    SamplesBuffer* readWavFile(QString fileName);
};

}

#endif // METRONOMETRACKNODE_H
