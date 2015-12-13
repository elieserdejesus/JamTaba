#include "AudioNode.h"
#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include "AudioPeak.h"
#include <cmath>
#include <cassert>
#include <QDebug>
#include "midi/MidiDriver.h"
#include <QMutexLocker>

#include "audio/Resampler.h"


using namespace Audio;

const double AudioNode::ROOT_2_OVER_2 = 1.414213562373095 *0.5;
const double AudioNode::PI_OVER_2 = 3.141592653589793238463 * 0.5;

//+++++++++++++++++

AudioNodeProcessor::AudioNodeProcessor()
    :bypassed(false){

}

void AudioNodeProcessor::setBypass(bool state){
    if(state != bypassed){
        bypassed = state;
    }
}

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

void FaderProcessor::process(const Audio::SamplesBuffer &in, SamplesBuffer &out, const Midi::MidiBuffer &midiBuffer){
    Q_UNUSED(midiBuffer);
    if(finished()){
        return;
    }
    out.set(in);//copy in to out
    float finalGain = currentGain + (gainStep * out.getFrameLenght());
    out.fade(currentGain, finalGain);
    currentGain = finalGain + gainStep;
    processedSamples += out.getFrameLenght();
}

bool FaderProcessor::finished(){
    return processedSamples >= totalSamplesToProcess;
}
//++++++++++++++++++++++++
//void AudioNode::deactivate(){
//    QMutexLocker locker(&mutex);
//    this->activated = false;
//}

//+++++++++++++++


void AudioNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer){
    Q_UNUSED(in);

    if(!isActivated()){
        return;
    }

    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght() );

    {
        QMutexLocker locker(&mutex);
        foreach (AudioNode* node, connections) {//ask connected nodes to generate audio
            node->processReplacing(internalInputBuffer, internalOutputBuffer, sampleRate, midiBuffer);
        }
    }

    internalOutputBuffer.set(internalInputBuffer);//if we have no plugins insert the input samples are just copied  to output buffer.

    if(!processors.isEmpty()){
        static SamplesBuffer tempInputBuffer(2);
        //process inserted plugins
        foreach (AudioNodeProcessor* processor, processors) {
            if(!processor->isBypassed() ){
                tempInputBuffer.setFrameLenght(internalOutputBuffer.getFrameLenght());
                tempInputBuffer.set(internalOutputBuffer);
                processor->process(tempInputBuffer, internalOutputBuffer, midiBuffer);
            }
        }
    }

    internalOutputBuffer.applyGain(gain, leftGain, rightGain, boost);

    lastPeak.update(internalOutputBuffer.computePeak());

    out.add(internalOutputBuffer);
}


AudioNode::AudioNode()
     :
      internalInputBuffer(2),
      internalOutputBuffer(2),
      lastPeak(0, 0),
      muted(false),
      soloed(false),
      activated(true),
      gain(1),
      boost(1),//no boost
      pan(0),//center
      leftGain(1.0),
      rightGain(1.0),
      resamplingCorrection(0)
{

}

int AudioNode::getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght) {
    double doubleValue = (double)sourceSampleRate*(double)outFrameLenght/(double)targetSampleRate;
    int intValue = (int)doubleValue;
    resamplingCorrection += doubleValue - intValue;
    if(qAbs(resamplingCorrection) > 1){
        intValue += resamplingCorrection;
        if(resamplingCorrection > 0){
            resamplingCorrection--;
        }
        else{
            resamplingCorrection++;
        }
    }
    return intValue;
}

Audio::AudioPeak AudioNode::getLastPeak() const{
    return this->lastPeak;
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
    emit panChanged(this->pan);
}

void AudioNode::setGain(float gainValue){
    this->gain = gainValue;
    emit gainChanged(this->gain);
}



void AudioNode::setMute(bool muteStatus){
    if(this->muted != muteStatus){
        this->muted = muteStatus;
        emit muteChanged(muteStatus);
    }
}

void AudioNode::setSolo(bool soloed){
    if(this->soloed != soloed){
        this->soloed = soloed;
        emit soloChanged(this->soloed);
    }
}

void AudioNode::reset(){
    setGain(1.0);
    setPan(0);
    setBoost(1.0);
    setMute(false);
    setSolo(false);
}

void AudioNode::updateGains(){
    double angle = pan * PI_OVER_2 * 0.5;
    leftGain = (float) (ROOT_2_OVER_2 * (cos(angle) - sin(angle)));
    rightGain = (float) (ROOT_2_OVER_2 * (cos(angle) + sin(angle)));
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

void AudioNode::addProcessor( AudioNodeProcessor* newProcessor)
{
    assert(newProcessor);
    processors.append(newProcessor);
}

void AudioNode::removeProcessor(AudioNodeProcessor* processor){
    assert(processor);
    processor->suspend();
    processors.removeOne(processor);

    delete processor;
}

void AudioNode::suspendProcessors(){
    foreach (AudioNodeProcessor* processor, processors) {
        processor->suspend();
    }
}

void AudioNode::updateProcessorsGui(){
    QMutexLocker locker(&mutex);
    foreach (AudioNodeProcessor* processor, processors) {
        processor->updateGui();
    }
}

void AudioNode::resumeProcessors(){
    foreach (AudioNodeProcessor* processor, processors) {
        processor->resume();
    }
}

//+++++++++++++++++++++++++++++++++++++++
OscillatorAudioNode::OscillatorAudioNode(float frequency, int sampleRate)
    :
        phase(0),
        phaseIncrement(2 * 3.1415 / sampleRate * frequency),
        sampleRate(sampleRate)
{

}

void OscillatorAudioNode::processReplacing(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer){
    Q_UNUSED(in)
    Q_UNUSED(sampleRate)
    Q_UNUSED(midiBuffer)
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
    : globalFirstInputIndex(0), channelIndex(parentChannelIndex),
        lastMidiActivity(0)
{
    Q_UNUSED(isMono)
    //setAudioInputSelection(0, isMono ? 1 : 2);
    setToNoInput();
}

LocalInputAudioNode::~LocalInputAudioNode(){
    //qWarning() << "Destrutor LocalInputAudioNode";
}

bool LocalInputAudioNode::isMono() const{
    return isAudio() && audioInputRange.isMono();
}

bool LocalInputAudioNode::isStereo() const{
    return isAudio() && audioInputRange.getChannels() == 2;
}

bool LocalInputAudioNode::isNoInput() const{
    return inputMode == DISABLED;
}

bool LocalInputAudioNode::isMidi() const{
    return inputMode == MIDI;// && midiDeviceIndex >= 0;
}

bool LocalInputAudioNode::isAudio() const{
    return inputMode == AUDIO;
}

void LocalInputAudioNode::setProcessorsSampleRate(int newSampleRate){
    foreach (Audio::AudioNodeProcessor* p, processors) {
        p->setSampleRate(newSampleRate);
    }
}

void LocalInputAudioNode::closeProcessorsWindows(){
    foreach (Audio::AudioNodeProcessor* p, processors) {
        p->closeEditor();
    }
}

void LocalInputAudioNode::addProcessor( AudioNodeProcessor* newProcessor){
    AudioNode::addProcessor(newProcessor);

    //if newProcessor is the first added processor and is a virtual instrument (VSTi) change the input selection to midi
    if(processors.size() == 1 &&  newProcessor->isVirtualInstrument()){
        if(!isMidi()){
            setMidiInputSelection(0, -1);//select the first midi device, all channels (-1)
        }
    }
}

void LocalInputAudioNode::setAudioInputSelection(int firstChannelIndex, int channelCount){
    audioInputRange = ChannelRange(firstChannelIndex, channelCount);
    if(audioInputRange.isMono())
        internalInputBuffer.setToMono();
    else
        internalInputBuffer.setToStereo();

    midiDeviceIndex = -1;//disable midi input
    inputMode = AUDIO;
}

void LocalInputAudioNode::setToNoInput(){
    audioInputRange = ChannelRange(-1, 0);//disable audio input
    midiDeviceIndex = -1;//disable midi input
    inputMode = DISABLED;
}

void LocalInputAudioNode::setMidiInputSelection(int midiDeviceIndex, int midiChannelIndex){
    audioInputRange = ChannelRange(-1, 0);//disable audio input
    this->midiDeviceIndex = midiDeviceIndex;
    this->midiChannelIndex = midiChannelIndex;
    inputMode = MIDI;
}

bool LocalInputAudioNode::isReceivingAllMidiChannels() const{
    if(inputMode == MIDI){
        return midiChannelIndex < 0 || midiChannelIndex > 16;
    }
    return false;
}

void LocalInputAudioNode::processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, const Midi::MidiBuffer& midiBuffer){
    Q_UNUSED(sampleRate);

    /* The input buffer (in) is a multichannel buffer. So, this buffer contains
     * all channels grabbed from soundcard inputs. If the user select a range of 4
     * input channels in audio preferences this buffer will contain 4 channels.
     *
     * A LocalInputAudioNode instance grab only your input range from this input SamplesBuffer.
     * Other LocalInputAudioNode instances will read other channels from input SamplesBuffer.
     */

    Midi::MidiBuffer filteredMidiBuffer(midiBuffer.getMessagesCount());
    internalInputBuffer.setFrameLenght(out.getFrameLenght());
    internalOutputBuffer.setFrameLenght(out.getFrameLenght());
    internalInputBuffer.zero();
    internalOutputBuffer.zero();

    if(!isNoInput()){
        if(isAudio()){//using audio input
            if(audioInputRange.isEmpty()){
                return;
            }
            int inChannelOffset = audioInputRange.getFirstChannel() - globalFirstInputIndex;
            internalInputBuffer.set(in, inChannelOffset, audioInputRange.getChannels());
        }
        else if(isMidi()){//just in case
            int total = midiBuffer.getMessagesCount();
            if(total > 0){
                for (int m = 0; m < total; ++m) {
                    Midi::MidiMessage message = midiBuffer.getMessage(m);
                    if( message.getDeviceIndex() == midiDeviceIndex && (isReceivingAllMidiChannels() || message.getChannel() == midiChannelIndex)){
                        filteredMidiBuffer.addMessage(message);

                        //save the midi activity peak value for notes or controls
                        if(message.isNote() || message.isControl()){
                            quint8 activityValue = message.getData2();
                            if(activityValue > lastMidiActivity){
                                lastMidiActivity = activityValue;
                            }
                        }
                    }
                }
            }
        }
    }
    AudioNode::processReplacing(in, out, sampleRate, filteredMidiBuffer);
}
//++++++++++++=
void LocalInputAudioNode::reset(){
    AudioNode::reset();
    setToNoInput();
}
