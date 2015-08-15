#pragma once
#include "AudioNode.h"
#include <QDialog>

namespace Audio {
class SamplesBuffer;
class Plugin;
}

namespace Vst {
    class VstHost;
}

namespace Audio {

class PluginDescriptor {

private:
    QString name;
    QString group;
    QString path;
public:
    PluginDescriptor(QString name, QString group);
    PluginDescriptor(QString name, QString group, QString path);
    PluginDescriptor();
    //PluginDescriptor(const PluginDescriptor&);
    //~PluginDescriptor();
    inline QString getName() const{return name;}
    inline QString getGroup() const{return group;}
    inline QString getPath() const{return path;}
    inline bool isValid() const{ return !name.isEmpty() && !group.isEmpty();}

    inline bool isVST() const{return group.toLower() == "vst";}
    inline bool isNative() const{return group.toLower() == "jamtaba";}

    static QString getPluginNameFromPath(QString path);

    QString toString() const;
    static PluginDescriptor fromString(QString);
};


//++++++++++++++++++++++++++++++++++++++++++++++++++
class PluginWindow : public QDialog
{
    Q_OBJECT

public:
    PluginWindow(Audio::Plugin* plugin);
    ~PluginWindow();
private:
    Audio::Plugin* plugin;
};
//+++++++++++++++++++++++
class Plugin : public Audio::AudioNodeProcessor{

public:
    explicit Plugin(QString name);
    virtual inline QString getName() const {return name;}
    virtual ~Plugin();
    virtual void setBypass(bool state);
    inline bool isBypassed() const{return bypassed;}
    virtual void openEditor(QPoint centerOfScreen) = 0;
    virtual void start() = 0;
    void setEditor(PluginWindow* editorWindow);
    PluginWindow* getEditor() const{return editorWindow;}
    inline bool hasEditorWindow() const{return editorWindow;}
    virtual QString getPath() const = 0;
    virtual QByteArray getSerializedData() const = 0;
    virtual void restoreFromSerializedData(QByteArray data) = 0;
protected:
    QString name;
    bool bypassed;
    PluginWindow* editorWindow;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++

class JamtabaDelay  : public Plugin{

public:
    explicit JamtabaDelay(int sampleRate);
    ~JamtabaDelay();
    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out, const Midi::MidiBuffer& midiBuffer);
    void setDelayTime(int delayTimeInMs);
    void setFeedback(float feedback);
    void setLevel(float level);

    inline float getDelayTime() const{return delayTimeInMs;}
    inline float getFeedback() const{return feedbackGain;}
    inline float getLevel() const{return level;}
    virtual void openEditor(QPoint centerOfScreen);
    virtual void start();
    inline QString getPath() const{return "";}

    virtual QByteArray getSerializedData() const;
    virtual void restoreFromSerializedData(QByteArray data);
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

