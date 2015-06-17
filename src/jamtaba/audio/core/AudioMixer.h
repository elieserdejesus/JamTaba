#pragma once

//#include <set>
#include <QMutex>

namespace Midi {
    class MidiBuffer;
}

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
    void removeNode(AudioNode &node);

    LocalInputAudioNode* getLocalInput() const{return inputNode;}

private:
    MainOutputAudioNode* mainOutNode;
    LocalInputAudioNode* inputNode;
    //QMutex mutex;
};
//+++++++++++++++++++++++



}
