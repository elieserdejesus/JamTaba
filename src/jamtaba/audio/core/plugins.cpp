#include "plugins.h"
#include "AudioDriver.h"
#include <QDebug>
#include <QMutexLocker>

using namespace Plugin;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PluginDescriptor::PluginDescriptor(QString name, QString group)
    :name(name), group(group)
{

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//a function in namespace Plugin to return all descriptors
std::vector<PluginDescriptor *> Plugin::getDescriptors(){
    static std::vector<Plugin::PluginDescriptor*> descriptors;
    descriptors.clear();
    //+++++++++++++++++
    descriptors.push_back(new Plugin::PluginDescriptor("Delay", "Jamtaba"));
    return descriptors;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
const int JamtabaDelay::MAX_DELAY_IN_SECONDS = 3;

JamtabaDelay::JamtabaDelay(int sampleRate)
    : Plugin("Delay", ""),

      internalBuffer(new Audio::SamplesBuffer(2, sampleRate  * MAX_DELAY_IN_SECONDS))//2 channels, 3 seconds delay

{
    delayTimeInSamples = sampleRate/2;//half second
    internalIndex = 0;
    feedbackGain = 0.3;//feedback start in this gain
    level = 1;
    internalBuffer->setFrameLenght(delayTimeInSamples);
    this->sampleRate = sampleRate;

}

JamtabaDelay::~JamtabaDelay(){
    delete internalBuffer;
    //delete mutex;
}

void JamtabaDelay::process(Audio::SamplesBuffer &buffer){
    float bufferValue = 0, internalValue = 0;
    for (int s = 0; s < buffer.getFrameLenght(); ++s) {
        for (int c = 0; c < buffer.getChannels(); ++c) {
            bufferValue = buffer.get(c, s);
            internalValue = internalBuffer->get(c, internalIndex);
            buffer.add(c, s, internalValue * level);//copy the internal sample to out buffer
            internalBuffer->set(c, internalIndex, bufferValue + internalValue * feedbackGain  ); //acumulate the sample in internal buffer
        }
        internalIndex = (internalIndex + 1) % internalBuffer->getFrameLenght();
    }
}

void JamtabaDelay::setDelayTime(int delayTimeInMs){

    if(delayTimeInMs > 0 ){
        if(delayTimeInMs > MAX_DELAY_IN_SECONDS * sampleRate){
            delayTimeInMs = MAX_DELAY_IN_SECONDS * sampleRate;
        }
        this->delayTimeInMs = delayTimeInMs;
        this->delayTimeInSamples = delayTimeInMs/1000.0 * sampleRate;
        this->internalBuffer->setFrameLenght(delayTimeInSamples);
    }
}

void JamtabaDelay::setFeedback(float feedback){
    this->feedbackGain = feedback;
}

void JamtabaDelay::setLevel(float level){
    if(level >= 0){
        this->level = level;
    }
}
