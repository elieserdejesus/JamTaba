#include "AudioNode.h"
#include "AudioDriver.h"
#include <cmath>
#include <QDebug>
#include "../midi/MidiDriver.h"

using namespace Audio;

const double AudioNode::root2Over2 = 1.414213562373095 *0.5;
const double AudioNode::piOver2 = 3.141592653589793238463 * 0.5;

//+++++++++++++++

FaderProcessor::FaderProcessor(float startGain, float endGain, int samplesToFade)
    : currentGain(startGain),
      startGain(startGain),
      gainStep((endGain-startGain)/samplesToFade),
      totalSamplesToProcess(samplesToFade),
      processedSamples(0)
{

}

void FaderProcessor::reset(){
    processedSamples = 0;
    currentGain = startGain;
}

void FaderProcessor::process(SamplesBuffer &buffer){
    if(finished()){
        return;
    }
    float finalGain = currentGain + (gainStep * buffer.getFrameLenght());
    buffer.fade(currentGain, finalGain);
    currentGain = finalGain + gainStep;
    processedSamples += buffer.getFrameLenght();
}

bool FaderProcessor::finished(){
    return processedSamples >= totalSamplesToProcess;
}
//++++++++++++++++++++++++
//+++++++++++++++

void AudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out)
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

    const float* peaks = internalBuffer->getPeaks();
    lastPeaks[0] = peaks[0]; lastPeaks[1] = peaks[1];

    out.add(*internalBuffer);
}

AudioNode::AudioNode()
    :muted(false),
      soloed(false),
      gain(1),
      pan(0)/*center*/,
      leftGain(1),
      rightGain(1)

{
    this->internalBuffer = new SamplesBuffer(2, AudioDriver::MAX_BUFFERS_LENGHT);
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

void AudioNode::removeProcessor(AudioNodeProcessor &processor){
    processors.erase(processors.find(&processor));
}

//+++++++++++++++++++++++++++++++++++++++
OscillatorAudioNode::OscillatorAudioNode(float frequency, int sampleRate)
    :phaseIncrement(2 * 3.1415 / sampleRate * frequency){
    this->phase = 0;
}

void OscillatorAudioNode::processReplacing(SamplesBuffer & /*in*/, SamplesBuffer &out){
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

void MainOutputAudioNode::processReplacing(SamplesBuffer&in, SamplesBuffer& out)
{
    static int soloedBuffersInLastProcess = 0;
    //--------------------------------------
    bool hasSoloedBuffers = soloedBuffersInLastProcess > 0;
    soloedBuffersInLastProcess = 0;
    for (const auto &node : connections) {
        bool canProcess = (!hasSoloedBuffers && !node->isMuted()) || (hasSoloedBuffers && node->isSoloed());
        if(canProcess){
            node->processReplacing(in, out);
        }
        else{//just discard the samples if node is muted, the internalBuffer is not copyed to out buffer
            internalBuffer->setFrameLenght(out.getFrameLenght());
            node->processReplacing(in, *internalBuffer);
        }
        if(node->isSoloed()){
            soloedBuffersInLastProcess++;
        }
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

void LocalInputAudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out)
{
    internalBuffer->setFrameLenght(out.getFrameLenght());
    internalBuffer->set(in);//copy into internal buffer
    AudioNode::processReplacing(in, out);

}

//++++++++++++=


