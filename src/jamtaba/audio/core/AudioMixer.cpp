#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "plugins.h"

using namespace Audio;
using namespace Plugin;

AudioMixer::AudioMixer()
{
    mainOutNode = new MainOutputAudioNode();
    inputNode = new LocalInputAudioNode();

    //disconnect to test
    inputNode->connect(*mainOutNode);
    inputNode->setGain(1);
}

void AudioMixer::addNode(AudioNode &node){
    node.connect(*mainOutNode);
}

AudioMixer::~AudioMixer()
{
    delete mainOutNode;
    delete inputNode;
}

void AudioMixer::process(SamplesBuffer &in, SamplesBuffer &out){
    mainOutNode->processReplacing(in, out);//mainOutNode ask all connected nodes to process
    //qDebug() << out.getPeaks()[0] << ", " << out.getPeaks()[1];
}
//++++++++++++++++++++++


