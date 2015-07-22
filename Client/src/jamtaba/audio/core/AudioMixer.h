#pragma once

#include <QList>
#include <QMutex>
#include <QMap>
#include "../SamplesBufferResampler.h"

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
    void process(const SamplesBuffer& in, SamplesBuffer& out, int sampleRate, bool attenuateAfterSumming = false);
    void addNode(AudioNode* node);
    void removeNode(AudioNode* node);

    //LocalInputAudioNode* getLocalInput() const{return inputNode;}

    inline void setSampleRate(int newSampleRate){this->sampleRate = newSampleRate;}

private:
    //LocalInputAudioNode* inputNode;

    QList<AudioNode*> nodes;
    int sampleRate;
    //QMutex mutex;
    QMap<AudioNode*, SamplesBufferResampler*> resamplers;
};
//+++++++++++++++++++++++



}
