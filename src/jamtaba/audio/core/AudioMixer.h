#pragma once

#include <set>

namespace Audio{

class AudioNode;
class AudioSamplesBuffer;
class MainOutputAudioNode;
class LocalInputAudioNode;

class AudioMixer
{
public:
    AudioMixer();
    ~AudioMixer();
    void process(AudioSamplesBuffer& in, AudioSamplesBuffer& out);
    void addNode(AudioNode &node);

private:
    MainOutputAudioNode* mainOutNode;
    LocalInputAudioNode* inputNode;
};
//+++++++++++++++++++++++



}
