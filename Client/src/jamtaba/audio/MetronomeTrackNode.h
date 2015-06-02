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
private:
    SamplesBuffer* clickSoundBuffer;
    unsigned int clickBufferOffset;


    SamplesBuffer* readWavFile(QString fileName);
};

}

#endif // METRONOMETRACKNODE_H
