#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#include <QList>
#include <QMutex>
#include <QMap>
#include <QScopedPointer>
#include "audio/SamplesBufferResampler.h"

namespace Midi {
class MidiMessage;
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
    explicit AudioMixer(int sampleRate);
    ~AudioMixer();
    void process(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const std::vector<Midi::MidiMessage> &midiBuffer, bool attenuateAfterSumming = false);
    void addNode(AudioNode *node);
    void removeNode(AudioNode *node);

    void setSampleRate(int newSampleRate);

private:
    QList<AudioNode *> nodes;
    int sampleRate;
    QMap<AudioNode *, SamplesBufferResampler *> resamplers;

};

inline void AudioMixer::setSampleRate(int newSampleRate)
{
    this->sampleRate = newSampleRate;
}

} // namespace

#endif
