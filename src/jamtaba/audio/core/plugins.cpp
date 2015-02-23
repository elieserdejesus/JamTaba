#include "plugins.h"
#include "AudioDriver.h"
#include <QDebug>

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
JamtabaDelay::JamtabaDelay()
    : Plugin("Delay", ""),
      internalBuffer(new Audio::SamplesBuffer(2, 44100 * 3))//2 channels, 3 seconds delay
{
    delayTimeInSamples = 44100/2;//half second
    internalIndex = 0;
    feedbackGain = 0.3;//feedback start in this gain
    internalBuffer->setFrameLenght(delayTimeInSamples);
}

JamtabaDelay::~JamtabaDelay(){
    delete internalBuffer;
}

void JamtabaDelay::process(Audio::SamplesBuffer &buffer){
    float bufferValue = 0, internalValue = 0;
    for (int s = 0; s < buffer.getFrameLenght(); ++s) {
        for (int c = 0; c < buffer.getChannels(); ++c) {
            bufferValue = buffer.get(c, s);
            internalValue = internalBuffer->get(c, internalIndex);
            buffer.add(c, s, internalValue);//copy the internal sample to out buffer
            internalBuffer->set(c, internalIndex, bufferValue + internalValue * feedbackGain  ); //acumulate the sample in internal buffer
        }
        internalIndex = (internalIndex + 1) % internalBuffer->getFrameLenght();
    }
}

/*
void JamtabaDelay::process(Audio::SamplesBuffer &outputBuffer){
    //acumulate
    int totalSamplesWrited = 0, samplesToWrite = 0;
    int bufferOffset = 0;
    int feedbackStartIndex = writePosition;
    do{
        samplesToWrite = std::min(internalBuffer->getFrameLenght() - writePosition, outputBuffer.getFrameLenght());
        internalBuffer->set(outputBuffer, bufferOffset, samplesToWrite, writePosition);
        bufferOffset += samplesToWrite;
        totalSamplesWrited += samplesToWrite;
        writePosition = (writePosition + samplesToWrite) % internalBuffer->getFrameLenght();
    }
    while(totalSamplesWrited < samplesToWrite);

    //copy delayed samples to outputBuffer
    float sampleValue=0;
    Audio::SamplesBuffer delayedBuffer(outputBuffer.getChannels(), outputBuffer.getFrameLenght());
    float feedback = 1;
    int repetitions = 0;
    int internalIndex = feedbackStartIndex;
    while(feedback > 0 && repetitions < 20){//using repetitions counter to avoid infinity loop
        for(int s=0; s < delayedBuffer.getFrameLenght(); ++s){
            for(int c=0; c < delayedBuffer.getChannels(); ++c){
                sampleValue = internalBuffer->get(c, internalIndex) * feedback;
                delayedBuffer.set(c, s, sampleValue);
            }
            internalIndex = (internalIndex + 1) % internalBuffer->getFrameLenght();
        }
        outputBuffer.add(delayedBuffer);
        float db = 20 * std::log10(feedback);
        feedback -= dbToLinear(-3);// (1 - feedbackGain);//TODO o mais fácil seria ter o feedback em db, e em cada repetição diminuir os dbs convertidos para linear
        repetitions++;
        internalIndex -= delayTimeInSamples;
        if(internalIndex < 0){
            internalIndex = internalBuffer->getFrameLenght() + internalIndex;
        }
        qDebug() << "feedback:" << feedback;
    }
    qDebug() << "Repetitions:" << repetitions << endl;

}
*/
