#ifndef AUDIO_NODE_H
#define AUDIO_NODE_H

#include <QSet>
#include <QMutex>
#include "SamplesBuffer.h"
#include "AudioDriver.h"
#include "midi/MidiMessage.h"
#include <QDebug>
#include <QList>

namespace Audio {

class AudioNodeProcessor;

class AudioNode : public QObject
{
    Q_OBJECT

public:
    AudioNode();
    virtual ~AudioNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate, std::vector<Midi::MidiMessage> &midiBuffer);

    virtual std::vector<Midi::MidiMessage> pullMidiMessagesGeneratedByPlugins() const;

    virtual void setMute(bool muted);

    void setSolo(bool soloed);

    bool isMuted() const;
    bool isSoloed() const;

    virtual bool connect(AudioNode &other);
    virtual bool disconnect(AudioNode &otherNode);

    virtual void addProcessor(AudioNodeProcessor *newProcessor, quint32 slotIndex);
    void removeProcessor(AudioNodeProcessor *processor);
    void suspendProcessors();
    void resumeProcessors();
    virtual void updateProcessorsGui();

    void setGain(float gainValue);
    void setBoost(float boostValue);

    float getBoost() const;
    float getGain() const;

    void setPan(float pan);
    float getPan() const;

    AudioPeak getLastPeak() const;

    void resetLastPeak();

    void setRmsWindowSize(int samples);

    void deactivate();

    void activate();

    virtual bool isActivated() const;

    virtual void reset(); // reset pan, gain, boost, etc

    static const quint8 MAX_PROCESSORS_PER_TRACK = 4;

protected:

    inline virtual void preFaderProcess(Audio::SamplesBuffer &out){ Q_UNUSED(out) } // called after process all input and plugins, and just before compute gain, pan and boost.
    inline virtual void postFaderProcess(Audio::SamplesBuffer &out){ Q_UNUSED(out) } // called after compute gain, pan and boost.

    int getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght);

    QSet<AudioNode *> connections;
    AudioNodeProcessor *processors[MAX_PROCESSORS_PER_TRACK];
    SamplesBuffer internalInputBuffer;
    SamplesBuffer internalOutputBuffer;

    mutable Audio::AudioPeak lastPeak;
    QMutex mutex; // used to protected connections manipulation because nodes can be added or removed by different threads

    // pan
    float pan;
    float leftGain;
    float rightGain;

private:
    AudioNode(const AudioNode &other);
    AudioNode &operator=(const AudioNode &other);

    bool muted;
    bool soloed;

    bool activated; // used when room stream is played. All tracks are disabled, except the room streamer.

    float gain;
    float boost;

    static const double ROOT_2_OVER_2;
    static const double PI_OVER_2;

    double resamplingCorrection;

    void updateGains();

signals:
    void gainChanged(float newGain);
    void panChanged(float newPan);
    void boostChanged(float newBoost);
    void muteChanged(bool muteStatus);
    void soloChanged(bool soloStatus);
};


inline void AudioNode::deactivate()
{
    activated = false;
}

inline void AudioNode::activate()
{
    activated = true;
}

inline bool AudioNode::isActivated() const
{
    return activated;
}

inline float AudioNode::getPan() const
{
    return pan;
}

inline float AudioNode::getBoost() const
{
    return boost;
}

inline float AudioNode::getGain() const
{
    return gain;
}

inline bool AudioNode::isMuted() const
{
    return muted;
}

inline bool AudioNode::isSoloed() const
{
    return soloed;
}


}//namespace

#endif
