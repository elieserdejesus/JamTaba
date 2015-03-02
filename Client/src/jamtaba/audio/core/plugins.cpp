#include "plugins.h"
#include "AudioDriver.h"
#include <QDebug>
#include <QMutexLocker>

using namespace Audio;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PluginDescriptor::PluginDescriptor(QString name, QString group)
    :name(name), group(group), path("")
{

}

PluginDescriptor::PluginDescriptor(QString name, QString group, QString path)
    :name(name), group(group), path(path)
{

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//a function in namespace Plugin to return all descriptors
std::vector<Audio::PluginDescriptor *> Audio::getPluginsDescriptors(){
    static std::vector<Audio::PluginDescriptor*> descriptors;
    descriptors.clear();
    //+++++++++++++++++
    descriptors.push_back(new Audio::PluginDescriptor("Delay", "Jamtaba"));

    descriptors.push_back(new Audio::PluginDescriptor("OldSkool test", "VST", "C:/Program Files (x86)/VSTPlugins/OldSkoolVerb.dll"));

    return descriptors;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Plugin::Plugin(QString name, QString file)
    :name(name), file(file), bypassed(false)
{

}

Plugin::~Plugin(){
    qDebug() << "Plugin destructor";
}

void Plugin::setBypass(bool state){
    if(state != bypassed){
        bypassed = state;
    }
}
//++++++++++++++++++++++++++++
PluginWindow::PluginWindow(QWidget *parent)
    :QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint)
{
    setAttribute( Qt::WA_DeleteOnClose, true );
    //resize ;
    //setWindowTitle(pluginGui->getPluginName());
}

PluginWindow::~PluginWindow()
{
    //pluginGui->setParent(nullptr);//avoid delete pluginGui instance
}


//+++++++++++++++++++++++++++++++++++++++++
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
    if(isBypassed()){
        return;
    }
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

void JamtabaDelay::openEditor(PluginWindow *w, QPoint p){

}
