#pragma once

#include "AudioNode.h"
#include <set>

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
