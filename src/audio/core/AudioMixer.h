#pragma once

#include <QList>
#include <QMutex>
#include <QMap>
#include "../SamplesBufferResampler.h"

#include <QScopedPointer>

namespace Midi {
    class MidiBuffer;
}

namespace Audio{

class AudioNode;
class SamplesBuffer;
//class MainOutputAudioNode;
class LocalInputAudioNode;



class AudioMixer
{
private:
    AudioMixer(const AudioMixer& other);
public:
    AudioMixer(int sampleRate);
    ~AudioMixer();
    void process(const SamplesBuffer& in, SamplesBuffer& out, int sampleRate, const Midi::MidiBuffer& midiBuffer, bool attenuateAfterSumming = false);
    void addNode(AudioNode* node);
    void removeNode(AudioNode* node);

    inline void setSampleRate(int newSampleRate){this->sampleRate = newSampleRate;}

private:
    QList<AudioNode*> nodes;
    int sampleRate;
    QMap<AudioNode*, SamplesBufferResampler*> resamplers;
};
//+++++++++++++++++++++++



}
