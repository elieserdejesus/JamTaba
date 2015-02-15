#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>

using namespace Audio;

AudioMixer::AudioMixer()
{
    mainOutNode = new MainOutputAudioNode();
    inputNode = new LocalInputAudioNode();

    //disconnect to test
    //inputNode->connect(*mainOutNode);
    inputNode->setGain(1);
    //inputNode->setPan(0);
}

void AudioMixer::addNode(AudioNode &node){
    node.connect(*mainOutNode);
}

AudioMixer::~AudioMixer()
{
    delete mainOutNode;
    delete inputNode;
}

void AudioMixer::process(AudioSamplesBuffer &in, AudioSamplesBuffer &out){
    mainOutNode->processReplacing(in, out);//mainOutNode ask all connected nodes to process
    //qDebug() << out.getPeaks()[0] << ", " << out.getPeaks()[1];
}
//++++++++++++++++++++++


