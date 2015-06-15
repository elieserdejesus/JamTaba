#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "plugins.h"
#include "../vst/VstPlugin.h"
#include "../midi/MidiDriver.h"
#include <QMutexLocker>

using namespace Audio;

AudioMixer::AudioMixer()
{
    mainOutNode = new MainOutputAudioNode();
    inputNode = new LocalInputAudioNode();

    //disconnect to test
    //inputNode->connect(*mainOutNode);
    //inputNode->setGain(1);

}

void AudioMixer::addNode(AudioNode &node){
    QMutexLocker locker(&mutex);
    node.connect(*mainOutNode);
}

void AudioMixer::removeNode(AudioNode &node){
    QMutexLocker locker(&mutex);
    node.disconnect(*mainOutNode);
}

AudioMixer::~AudioMixer()
{
    QMutexLocker locker(&mutex);
    delete mainOutNode;
    mainOutNode = nullptr;
    delete inputNode;
    inputNode = nullptr;
}

void AudioMixer::process(SamplesBuffer &in, SamplesBuffer &out){
    QMutexLocker locker(&mutex);
    if(mainOutNode){
        mainOutNode->processReplacing(in, out);//mainOutNode ask all connected nodes to process
    }

}

//++++++++++++++++++++++


