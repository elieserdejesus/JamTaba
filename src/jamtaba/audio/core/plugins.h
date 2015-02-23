#pragma once
#include "AudioNode.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <QObject>

namespace Audio {
class SamplesBuffer;
}

namespace Plugin {

class PluginDescriptor : public QObject{
    Q_OBJECT
private:
    const QString name;
    const QString group;
public:
    PluginDescriptor(QString name, QString group);
    inline QString getName() const{return name;}
    inline QString getGroup() const{return group;}
};



std::vector<PluginDescriptor*> getDescriptors();

//+++++++++++++++++++++++++++++++++++++++++

class JamtabaDelay : public Audio::Plugin{
public:
    JamtabaDelay();
    ~JamtabaDelay();
    virtual void process(Audio::SamplesBuffer &buffer);
private:
    int delayTimeInSamples;
    float feedbackGain;
    int internalIndex;
    //int readPosition;
    Audio::SamplesBuffer* internalBuffer;
};

}

