#include "AudioNode.h"
#include "AudioDriver.h"
#include <cmath>
#include <QDebug>

void AudioNode::processReplacing(AudioSamplesBuffer &in, AudioSamplesBuffer &out)
{
    foreach (AudioNode* node, connections) {
        node->processReplacing(in, out);
    }
}

bool AudioNode::connect(AudioNode* otherNode){
    otherNode->connections.insert(  this );
    return true;
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
    out.add(in);//copy the input samples to output buffer. The output buffer values are erased
}

//++++++++++++=


GainNode::GainNode(float initialGain)
{
    this->gain = initialGain;
    this->internalBuffer = new AudioSamplesBuffer(2, AudioDriver::MAX_BUFFERS_LENGHT);
}

GainNode::~GainNode()
{
    delete this->internalBuffer;
}

float GainNode::getGain(){
    return gain;
}

void GainNode::setGain(float gain){
    if(gain < 0){
        gain = 0;
    }
    this->gain = gain;
}

void GainNode::processReplacing(AudioSamplesBuffer&in, AudioSamplesBuffer& out)
{
    internalBuffer->setFrameLenght(out.getFrameLenght());
    AudioNode::processReplacing(in, *internalBuffer);//child nodes render to internalBuffer
    internalBuffer->applyGain(gain);
    out.add(*internalBuffer);//copy the changed samples to out buffer
}
