#include "AudioNode.h"
#include "AudioDriver.h"
#include <cmath>
#include <QDebug>

using namespace Audio;

const double AudioNode::root2Over2 = 1.414213562373095 *0.5;
const double AudioNode::piOver2 = 3.141592653589793238463 * 0.5;

//+++++++++++++++
void AudioNode::processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out)
{
    internalBuffer->setFrameLenght(out.getFrameLenght());

    foreach (AudioNode* node, connections) {
        node->processReplacing(in, *internalBuffer);
    }

    //plugins
    foreach (AudioNodeProcessor* processor, processors) {
        processor->process(*internalBuffer);
    }
    internalBuffer->applyGain(gain, leftGain, rightGain);
    out.add(*internalBuffer);
}

AudioNode::AudioNode()
{
    this->internalBuffer = new AudioSamplesBuffer(2, AudioDriver::MAX_BUFFERS_LENGHT);
    this->gain = 1;
    this->pan = 0;//center
    this->leftGain = this->rightGain = 1;//pan gains
}

void AudioNode::setPan(float pan) {
    if (pan < -1) {
        pan = -1;
    }
    if (pan > 1) {
        pan = 1;
    }
    this->pan = pan;
    updateGains();
}

AudioNode::~AudioNode()
{
    foreach (AudioNodeProcessor* processor, processors) {
        delete processor;
    }
    processors.clear();

    delete internalBuffer;
}

bool AudioNode::connect(AudioNode& otherNode) {
    otherNode.connections.insert(this );
    return true;
}

void AudioNode::addProcessor( AudioNodeProcessor& newProcessor)
{
    processors.insert(&newProcessor);
}

//+++++++++++++++++++++++++++++++++++++++
OscillatorAudioNode::OscillatorAudioNode(float frequency, int sampleRate)
    :phaseIncrement(2 * 3.1415 / sampleRate * frequency){
    this->phase = 0;
}

void OscillatorAudioNode::processReplacing(AudioSamplesBuffer & /*in*/, AudioSamplesBuffer &out){
    int frames = out.getFrameLenght();
    int outChannels = out.getChannels();
    for (int i = 0; i < frames; ++i) {
        float sample = sin(this->phase);
        for (int c = 0; c < outChannels; ++c) {
            out.add(c,i, sample);
        }
        this->phase += phaseIncrement;
    }
}
//+++++++++++++++++++++++++++++++++++++++

void MainOutputAudioNode::processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out)
{
    foreach (AudioNode* node, connections) {
        node->processReplacing(in, out);
    }

    int nodesConnected = connections.size();
    if(nodesConnected > 1){//attenuate
        out.applyGain( 1.0/nodesConnected );
    }

}

//++++++++++++++++++++++++++++++++++++++++++++++
LocalInputAudioNode::LocalInputAudioNode(int firstInputIndex, bool isMono)
{
    this->firstInputIndex = firstInputIndex;
    this->mono = isMono;
}

void LocalInputAudioNode::processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out)
{
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->set(in);//copy in to internal buffer
    AudioNode::processReplacing(in, out);

}

//++++++++++++=


