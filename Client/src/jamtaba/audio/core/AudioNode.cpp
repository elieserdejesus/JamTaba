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
        foreach (AudioNode* node, connections) {//ask connected nodes to generate audio
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
      internalBuffer(2),
      lastPeak(0, 0),
      activated(true),
      muted(false),
      soloed(false),
      gain(1),
      pan(0)/*center*/,
      leftGain(1.0),
      rightGain(1.0)

{

}

int AudioNode::getInputResamplingLength(int targetSampleRate, int outFrameLenght) const{
    double factor = (double)getSampleRate()/(double)targetSampleRate;
    double doubleLenght = (double)outFrameLenght * factor;// + resamplingCorrection;
    int intLenght = std::ceil(doubleLenght);


//    double reverseFactor = (double)targetSampleRate/(double)getSampleRate();
//    int resampledLenght = intLenght * reverseFactor;
//    resamplingCorrection += (double)(intLenght * reverseFactor) - resampledLenght;
//    qDebug() << "correction:" << resamplingCorrection << "intLenght:" << intLenght << "doubleLenght: " << doubleLenght << "outFrameLenght" << outFrameLenght;

    return intLenght;
}

//void AudioNode::pushBackDiscardedSamples(const SamplesBuffer &buffer, unsigned int discardedSamples){
//    if(discardedSamples < buffer.getFrameLenght()){
//        discardedBuffer.setFrameLenght(discardedSamples);
//        discardedBuffer.set(buffer, buffer.getFrameLenght() - discardedSamples, discardedSamples, 0);
//    }
//    else{
//        qCritical() << "too many discarded samples?";
//    }
//}

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

void AudioNode::updateGains(){
    double angle = pan * piOver2 * 0.5;
    leftGain = (float) (root2Over2 * (cos(angle) - sin(angle)));
    rightGain = (float) (root2Over2 * (cos(angle) + sin(angle)));
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
    :
        phase(0),
        phaseIncrement(2 * 3.1415 / sampleRate * frequency),
        sampleRate(sampleRate)
{

}

void OscillatorAudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out){
    Q_UNUSED(in);
    int frames = out.getFrameLenght();
    int outChannels = out.getChannels();
    for (int i = 0; i < frames; ++i) {
        float sample = sin(this->phase) * getGain();
        for (int c = 0; c < outChannels; ++c) {
            out.set(c, i + 0, sample);//mesmo zerando o offset o audio ainda fica interrompido
        }
        this->phase += phaseIncrement;
    }

    /*
    int frames = out.getFrameLenght();
    int outChannels = out.getChannels();
    internalBuffer.setFrameLenght(frames);
    internalBuffer.zero();
    for (int i = 0; i < frames; ++i) {
        float sample = sin(this->phase);
        for (int c = 0; c < outChannels; ++c) {
            internalBuffer.add(c, i, sample);
        }
        this->phase += phaseIncrement;
    }
    Audio::AudioNode::processReplacing(in, out, outOffset);
    */
}
//++++++++++++++++++++++++++++++++++++++++++++++
LocalInputAudioNode::LocalInputAudioNode(int parentChannelIndex, bool isMono)
    :channelIndex(parentChannelIndex),  globalFirstInputIndex(0)
{
    setAudioInputSelection(0, isMono ? 1 : 2);
}

void LocalInputAudioNode::setAudioInputSelection(int firstChannelIndex, int channelCount){
    audioInputRange = ChannelRange(firstChannelIndex, channelCount);
    if(audioInputRange.isMono())
        internalBuffer.setToMono();
    else
        internalBuffer.setToStereo();

    midiDeviceIndex = -1;//disable midi input
}

void LocalInputAudioNode::setToNoInput(){
    audioInputRange = ChannelRange(-1, 0);//disable audio input
    midiDeviceIndex = -1;//disable midi input
}

void LocalInputAudioNode::setMidiInputSelection(int midiDeviceIndex){
    audioInputRange = ChannelRange(-1, 0);//disable audio input
    this->midiDeviceIndex = midiDeviceIndex;
}

void LocalInputAudioNode::processReplacing(SamplesBuffer &in, SamplesBuffer &out){
    /* The input buffer (in) is a multichannel buffer. So, this buffer contains
     * all channels grabbed from soundcard inputs. If the user select a range of 4
     * input channels in audio preferences this buffer will contain 4 channels.
     *
     * A LocalInputAudioNode instance grab only your input range from this input SamplesBuffer.
     * Other LocalInputAudioNode instances will read other channels from input SamplesBuffer.
     */

    if(audioInputRange.isEmpty()){
        return;
    }
    internalBuffer.setFrameLenght(out.getFrameLenght());
    int inChannelOffset = audioInputRange.getFirstChannel() - globalFirstInputIndex;
    internalBuffer.set(in, inChannelOffset, audioInputRange.getChannels());

    AudioNode::processReplacing(in, out);
}
//++++++++++++=
