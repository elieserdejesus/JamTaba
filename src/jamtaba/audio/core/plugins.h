#pragma once
#include "AudioNode.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <QObject>
#include <QMutex>
#include <QDialog>

namespace Audio {
class SamplesBuffer;
class Plugin;
}

namespace Vst {
    class VstHost;
}

namespace Audio {

class PluginDescriptor : public QObject{
    Q_OBJECT
private:
    const QString name;
    const QString group;
    const QString path;
public:
    PluginDescriptor(QString name, QString group);
    PluginDescriptor(QString name, QString group, QString path);
    inline QString getName() const{return name;}
    inline QString getGroup() const{return group;}
    inline QString getPath() const{return path;}
};

std::vector<PluginDescriptor*> getPluginsDescriptors(Vst::VstHost *host);

//++++++++++++++++++++++++++++++++++++++++++++++++++
class PluginWindow : public QDialog
{
    Q_OBJECT

public:
    static PluginWindow* getWindow(QWidget* parent, Audio::Plugin* plugin);
    ~PluginWindow();
    //void setPlugin(Audio::Plugin* plugin);
    //Audio::Plugin* getPlugin() const{return plugin;}

private:
    static QMap<Audio::Plugin*, PluginWindow*> windows;
    PluginWindow(QWidget* parent, Audio::Plugin* plugin);
    Audio::Plugin* plugin;
};
//+++++++++++++++++++++++
class Plugin : public Audio::AudioNodeProcessor{

public:
    Plugin(QString name);
    virtual inline QString getName() const {return name;}
    virtual ~Plugin();
    virtual void setBypass(bool state);
    inline bool isBypassed() const{return bypassed;}
    virtual void openEditor(PluginWindow* w, QPoint p) = 0;
    virtual void start(int sampleRate, int bufferSize) = 0;
protected:
    QString name;
    bool bypassed;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++

class JamtabaDelay  : public Plugin{

public:
    JamtabaDelay(int sampleRate);
    ~JamtabaDelay();
    virtual void process(Audio::SamplesBuffer &buffer, Midi::MidiBuffer& midiIn);
    void setDelayTime(int delayTimeInMs);
    void setFeedback(float feedback);
    void setLevel(float level);

    inline float getDelayTime() const{return delayTimeInMs;}
    inline float getFeedback() const{return feedbackGain;}
    inline float getLevel() const{return level;}
    virtual void openEditor(PluginWindow *, QPoint p);
    virtual void start(int sampleRate, int bufferSize);
private:
    void setSampleRate(int newSampleRate);

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

