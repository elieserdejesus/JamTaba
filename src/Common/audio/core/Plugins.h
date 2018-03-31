#ifndef PLUGINS_H
#define PLUGINS_H

#include "AudioNodeProcessor.h"
#include "PluginDescriptor.h"

class QDialog;

namespace audio {
    class SamplesBuffer;
    class Plugin;
}

namespace Jamtaba {
    class VstHost;
}

namespace audio {

class Plugin : public AudioNodeProcessor
{
    Q_OBJECT

public:
    explicit Plugin(const PluginDescriptor &pluginDescriptor);
    virtual ~Plugin();

    virtual QString getName() const;

    virtual void closeEditor();
    virtual void start() = 0;
    virtual QString getPath() const = 0;
    virtual QByteArray getSerializedData() const = 0;
    virtual void restoreFromSerializedData(const QByteArray &data) = 0;

    PluginDescriptor getDescriptor() const;

protected:
    QString name;
    QDialog *editorWindow;
    PluginDescriptor descriptor;

private slots:
    void editorDialogFinished();
};

inline PluginDescriptor Plugin::getDescriptor() const
{
    return descriptor;
}

inline QString Plugin::getName() const
{
    return name;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class JamtabaDelay : public Plugin
{
public:
    explicit JamtabaDelay(int sampleRate);
    ~JamtabaDelay();
    void process(const SamplesBuffer &in, SamplesBuffer &out, std::vector<midi::MidiMessage> &midiBuffer) override;
    void setDelayTime(int delayTimeInMs);
    void setFeedback(float feedback);
    void setLevel(float level);

    float getDelayTime() const;

    float getFeedback() const;

    float getLevel() const;

    void openEditor(const QPoint &centerOfScreen) override;
    void updateGui() override;

    void start() override;
    QString getPath() const override;

    QByteArray getSerializedData() const override;
    void restoreFromSerializedData(const QByteArray &data) override;

    void suspend() override;

    void resume() override;

private:
    void setSampleRate(int newSampleRate) override;

    static const int MAX_DELAY_IN_SECONDS;
    int delayTimeInSamples;
    float delayTimeInMs;
    float feedbackGain;
    float level;
    int internalIndex;
    int sampleRate;
    audio::SamplesBuffer *internalBuffer;
};

inline void JamtabaDelay::suspend()
{

}

inline void JamtabaDelay::resume()
{

}

inline QString JamtabaDelay::getPath() const
{
    return "";
}

inline void JamtabaDelay::updateGui()
{

}

inline float JamtabaDelay::getDelayTime() const
{
    return delayTimeInMs;
}

inline float JamtabaDelay::getFeedback() const
{
    return feedbackGain;
}

inline float JamtabaDelay::getLevel() const
{
    return level;
}

} // namespace

#endif
