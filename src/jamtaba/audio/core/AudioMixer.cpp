#include "AudioMixer.h"
#include "AudioNode.h"
#include <QDebug>
#include "plugins.h"
#include "../vst/VstPlugin.h"
#include "../midi/MidiDriver.h"

using namespace Audio;

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

void AudioMixer::process(SamplesBuffer &in, SamplesBuffer &out, Midi::MidiBuffer &midiIn){
    mainOutNode->processReplacing(in, out, midiIn);//mainOutNode ask all connected nodes to process
    //qDebug() << out.getPeaks()[0] << ", " << out.getPeaks()[1];
}

//++++++++++++++++++++++


