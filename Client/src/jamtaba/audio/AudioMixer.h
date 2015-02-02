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

private:
    MainOutputAudioNode* mainOutNode;
    LocalInputAudioNode* inputNode;
};
//+++++++++++++++++++++++

class AudioMixerTrack{
private:

public:
    AudioMixerTrack();
    virtual ~AudioMixerTrack();

};

}
