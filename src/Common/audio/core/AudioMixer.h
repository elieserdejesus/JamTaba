#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <QList>
#include <QMutex>
#include <QMap>
#include <QScopedPointer>
#include "audio/SamplesBufferResampler.h"

namespace Midi {
class MidiMessageBuffer;
}

namespace Audio {
class AudioNode;
class SamplesBuffer;
class LocalInputNode;

class AudioMixer
{
private:
    AudioMixer(const AudioMixer &other);
public:
    AudioMixer(int sampleRate);
    ~AudioMixer();
    void process(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                 const Midi::MidiMessageBuffer &midiBuffer, bool attenuateAfterSumming = false);
    void addNode(AudioNode *node);
    void removeNode(AudioNode *node);

    inline void setSampleRate(int newSampleRate)
    {
        this->sampleRate = newSampleRate;
    }

private:
    QList<AudioNode *> nodes;
    int sampleRate;
    QMap<AudioNode *, SamplesBufferResampler *> resamplers;
};
// +++++++++++++++++++++++
}

#endif
