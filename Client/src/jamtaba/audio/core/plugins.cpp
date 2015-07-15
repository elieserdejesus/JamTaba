#include "plugins.h"
#include "AudioDriver.h"
#include "SamplesBuffer.h"
#include <QDebug>
#include <QMutexLocker>
#include "../../audio/vst/PluginFinder.h"

using namespace Audio;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PluginDescriptor::PluginDescriptor()
    :name(""), group(""), path(""){
}

PluginDescriptor::PluginDescriptor(QString name, QString group)
    :name(name), group(group), path("")
{

}

//PluginDescriptor::PluginDescriptor(const PluginDescriptor &other)
//    :name(other.name), group(other.group), path(other.path){

//}

PluginDescriptor::PluginDescriptor(QString name, QString group, QString path)
    :name(name), group(group), path(path)
{

}

QString PluginDescriptor::toString() const{
    return name + ";" + group + ";" + path;
}

PluginDescriptor PluginDescriptor::fromString(QString str){
    QStringList parts = str.split(";");
    return PluginDescriptor(parts.at(0), parts.at(1), parts.at(2));
}

QString PluginDescriptor::getPluginNameFromPath(QString path){
    QString name = QFile(path).fileName();
    int indexOfDirSeparator = name.lastIndexOf("/");
    if(indexOfDirSeparator >= 0){
        name = name.right(name.length() - indexOfDirSeparator - 1);
    }
    int indexOfPoint = name.lastIndexOf(".");
    if(indexOfPoint > 0){
        name = name.left(indexOfPoint);
    }
    return name;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//a function in namespace Plugin to return all descriptors

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Plugin::Plugin(QString name)
    :name(name), bypassed(false), editorWindow(nullptr)
{

}

void Plugin::setEditor(PluginWindow *editorWindow){
    if(this->editorWindow){
        editorWindow->deleteLater();
    }
    this->editorWindow = editorWindow;
}

Plugin::~Plugin(){
    qDebug() << "Plugin destructor";
    if(editorWindow){
       delete editorWindow;
    }
}

void Plugin::setBypass(bool state){
    if(state != bypassed){
        bypassed = state;
    }
}
//++++++++++++++++++++++++++++
PluginWindow::PluginWindow( Plugin *plugin)
    :QDialog(nullptr, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
      plugin(plugin)
{
    //setAttribute( Qt::WA_DeleteOnClose, true );
    setWindowTitle(plugin->getName());
}

PluginWindow::~PluginWindow(){
    qDebug() << "PLugin window destructor";
}

//+++++++++++++++++++++++++++++++++++++++++
const int JamtabaDelay::MAX_DELAY_IN_SECONDS = 3;

JamtabaDelay::JamtabaDelay(int sampleRate)
    : Plugin("Delay"), delayTimeInMs(0),

      internalBuffer(new Audio::SamplesBuffer(2))//2 channels, 3 seconds delay

{
    internalIndex = 0;
    feedbackGain = 0.3f;//feedback start in this gain
    level = 1;
    setSampleRate(sampleRate);
}

void JamtabaDelay::setSampleRate(int newSampleRate){
    this->sampleRate = newSampleRate;
    delayTimeInSamples = this->sampleRate/2;//half second
    internalBuffer->setFrameLenght(delayTimeInSamples);
}

JamtabaDelay::~JamtabaDelay(){
    delete internalBuffer;
    //delete mutex;
}

void JamtabaDelay::start(int sampleRate, int /*bufferSize*/){
    if(sampleRate != this->sampleRate){
        setSampleRate(sampleRate);
    }
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

void JamtabaDelay::openEditor(QPoint p){
    //Q_UNUSED(w);
    Q_UNUSED(p);
}
