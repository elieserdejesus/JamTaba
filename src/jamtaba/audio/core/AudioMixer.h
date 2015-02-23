#pragma once

#include <set>

namespace Audio{

class AudioNode;
class SamplesBuffer;
class MainOutputAudioNode;
class LocalInputAudioNode;

class AudioMixer
{
public:
    AudioMixer();
    ~AudioMixer();
    void process(SamplesBuffer& in, SamplesBuffer& out);
    void addNode(AudioNode &node);

private:
    MainOutputAudioNode* mainOutNode;
    LocalInputAudioNode* inputNode;
};
//+++++++++++++++++++++++



}
