#ifndef PLUGINS_H
#define PLUGINS_H

#include "AudioNodeProcessor.h"
#include "PluginDescriptor.h"

class QDialog;

namespace Audio {
class SamplesBuffer;
class Plugin;
}

namespace Jamtaba {
class VstHost;
}

namespace Audio {
// ++++++++++++++++++++++++++++++++++++++++++++++++++
class Plugin : public Audio::AudioNodeProcessor
{
    Q_OBJECT
public:
    explicit Plugin(const PluginDescriptor &pluginDescriptor);
    virtual inline QString getName() const
    {
        return name;
    }

    virtual ~Plugin();

    // virtual void openEditor(QPoint centerOfScreen);
    virtual void closeEditor();
    virtual void start() = 0;
    virtual QString getPath() const = 0;
    virtual QByteArray getSerializedData() const = 0;
    virtual void restoreFromSerializedData(const QByteArray &data) = 0;

    inline PluginDescriptor getDescriptor() const { return descriptor; }

protected:
    QString name;
    QDialog *editorWindow;
    PluginDescriptor descriptor;

private slots:
    void editorDialogFinished();
};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class JamtabaDelay : public Plugin
{
public:
    explicit JamtabaDelay(int sampleRate);
    ~JamtabaDelay();
    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                         const QList<Midi::MidiMessage> &midiBuffer);
    void setDelayTime(int delayTimeInMs);
    void setFeedback(float feedback);
    void setLevel(float level);

    inline float getDelayTime() const
    {
        return delayTimeInMs;
    }

    inline float getFeedback() const
    {
        return feedbackGain;
    }

    inline float getLevel() const
    {
        return level;
    }

    void openEditor(const QPoint &centerOfScreen) override;
    void updateGui() override
    {
    }

    virtual void start();
    inline QString getPath() const
    {
        return "";
    }

    QByteArray getSerializedData() const override;
    void restoreFromSerializedData(const QByteArray &data) override;

    void suspend()
    {
    }

    void resume()
    {
    }

private:
    void setSampleRate(int newSampleRate);

    static const int MAX_DELAY_IN_SECONDS;
    int delayTimeInSamples;
    float delayTimeInMs;
    float feedbackGain;
    float level;
    int internalIndex;
    int sampleRate;
    Audio::SamplesBuffer *internalBuffer;
};
}

#endif
