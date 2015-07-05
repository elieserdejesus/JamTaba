#include "AudioNode.h"
#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include "AudioPeak.h"
#include <cmath>
#include <QDebug>
#include "../midi/MidiDriver.h"
#include <QMutexLocker>
#include "../audio/Resampler.h"

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
void AudioNode::deactivate(){
    QMutexLocker locker(&mutex);
    this->activated = false;
}

//+++++++++++++++

bool AudioNode::needResamplingFor(int targetSampleRate) const{
    Q_UNUSED(targetSampleRate);
    return false;
}

void AudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out){
    if(!activated){
        return;
    }
    internalBuffer.setFrameLenght(out.getFrameLenght());
    {
        QMutexLocker locker(&mutex);
        foreach (AudioNode* node, connections) {
            node->processReplacing(in, internalBuffer);
        }
    }

    //plugins
    foreach (AudioNodeProcessor* processor, processors) {
        processor->process(internalBuffer);
    }
    internalBuffer.applyGain(gain, leftGain, rightGain);

    lastPeak.update(internalBuffer.computePeak());

    out.add(internalBuffer);
}


AudioNode::AudioNode()
     :
      activated(true),
      muted(false),
      soloed(false),
      gain(1),
      pan(0)/*center*/,
      leftGain(1.0),
      rightGain(1.0),
      internalBuffer(2),
      lastPeak(0, 0),
      resamplingCorrection(0)
{

}

int AudioNode::getInputResamplingLength(int targetSampleRate, int outFrameLenght) const{
    double factor = (double)getSampleRate()/targetSampleRate;
    double doubleLenght = outFrameLenght * factor + resamplingCorrection;
    int intLenght = std::round(doubleLenght);
    resamplingCorrection = intLenght - doubleLenght;
    return intLenght;
}

Audio::AudioPeak AudioNode::getLastPeak(bool resetPeak) const{
    AudioPeak peak = this->lastPeak;
    if(resetPeak){
        this->lastPeak.zero();
    }

    return peak;
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

    //sdelete internalBuffer;
}

bool AudioNode::connect(AudioNode& other) {
    QMutexLocker(&(other.mutex));
    other.connections.insert(this);
    return true;
}

bool AudioNode::disconnect(AudioNode &otherNode){
    QMutexLocker(&(otherNode.mutex));
    otherNode.connections.remove(this );
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
    :   phaseIncrement(2 * 3.1415 / sampleRate * frequency),
        phase(0),
        sampleRate(sampleRate)
{

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

//++++++++++++++++++++++++++++++++++++++++++++++
LocalInputAudioNode::LocalInputAudioNode( int firstInputIndex, bool isMono)
{
    this->firstInputIndex = firstInputIndex;
    this->mono = isMono;
}

void LocalInputAudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out)
{
    internalBuffer.setFrameLenght(out.getFrameLenght());
    internalBuffer.set(in);//copy into internal buffer
    AudioNode::processReplacing(in, out);

}

//++++++++++++=


