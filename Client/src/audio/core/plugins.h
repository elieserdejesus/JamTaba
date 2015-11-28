#pragma once
#include "AudioNode.h"

class QDialog;

namespace Audio {
class SamplesBuffer;
class Plugin;
}

namespace Jamtaba {
    class VstHost;
}

namespace Audio {


//++++++++++++++++++++++++++++++++++++++++++++++++++
class Plugin : public Audio::AudioNodeProcessor{
    Q_OBJECT
public:
    explicit Plugin(QString name);
    virtual inline QString getName() const {return name;}
    virtual ~Plugin();

    //virtual void openEditor(QPoint centerOfScreen);
    virtual void closeEditor();
    virtual void start() = 0;
    virtual QString getPath() const = 0;
    virtual QByteArray getSerializedData() const = 0;
    virtual void restoreFromSerializedData(QByteArray data) = 0;
protected:
    QString name;

    QDialog* editorWindow;
private slots:
    void editorDialogFinished();
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
    virtual void updateGui(){}
    //virtual void closeEditor();
    virtual void start();
    inline QString getPath() const{return "";}

    virtual QByteArray getSerializedData() const;
    virtual void restoreFromSerializedData(QByteArray data);

    void suspend(){}
    void resume(){}
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

