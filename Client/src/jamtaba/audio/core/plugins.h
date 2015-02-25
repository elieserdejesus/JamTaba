#pragma once
#include "AudioNode.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <QObject>
#include <QMutex>

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

class JamtabaDelay  : public Audio::Plugin{

public:
    JamtabaDelay(int sampleRate);
    ~JamtabaDelay();
    virtual void process(Audio::SamplesBuffer &buffer);
    void setDelayTime(int delayTimeInMs);
    void setFeedback(float feedback);
    void setLevel(float level);

    inline float getDelayTime() const{return delayTimeInMs;}
    inline float getFeedback() const{return feedbackGain;}
    inline float getLevel() const{return level;}

private:
    static const int MAX_DELAY_IN_SECONDS;// = 3;//3 seconds
    int delayTimeInSamples;
    float delayTimeInMs;
    float feedbackGain;
    float level;
    int internalIndex;
    int sampleRate;
    //QMutex mutex;
    //int readPosition;
    Audio::SamplesBuffer* internalBuffer;
};

}

