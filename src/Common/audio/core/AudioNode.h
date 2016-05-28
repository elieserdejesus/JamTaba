#ifndef AUDIO_NODE_H
#define AUDIO_NODE_H

#include <QSet>
#include <QMutex>
#include "SamplesBuffer.h"
#include "AudioDriver.h"
#include "midi/MidiMessage.h"
#include <QDebug>
#include <QList>

namespace Midi   {
class MidiMessageBuffer;
}

namespace Audio {

class AudioNodeProcessor;

class AudioNode : public QObject
{
    Q_OBJECT
public:
    AudioNode();
    virtual ~AudioNode();

    virtual void processReplacing(const SamplesBuffer &in, SamplesBuffer &out, int sampleRate,
                                  const Midi::MidiMessageBuffer &midiBuffer);

    virtual QList<Midi::MidiMessage> pullMidiMessagesGeneratedByPlugins() const;

    virtual void setMute(bool muted);

    void setSolo(bool soloed);

    inline bool isMuted() const
    {
        return muted;
    }

    inline bool isSoloed() const
    {
        return soloed;
    }

    virtual bool connect(AudioNode &other);
    virtual bool disconnect(AudioNode &otherNode);

    virtual void addProcessor(AudioNodeProcessor *newProcessor, quint32 slotIndex);
    void removeProcessor(AudioNodeProcessor *processor);
    void suspendProcessors();
    void resumeProcessors();
    virtual void updateProcessorsGui();

    void setGain(float gainValue);
    inline void setBoost(float boostValue)
    {
        this->boost = boostValue;
    }

    inline float getBoost() const
    {
        return boost;
    }

    inline float getGain() const
    {
        return gain;
    }

    void setPan(float pan);
    inline float getPan() const
    {
        return pan;
    }

    AudioPeak getLastPeak() const;

    void resetLastPeak();

    void setRmsWindowSize(int samples);

    inline void deactivate()
    {
        activated = false;
    }

    inline void activate()
    {
        activated = true;
    }

    virtual inline bool isActivated() const
    {
        return activated;
    }

    virtual void reset();// reset pan, gain, boost, etc

    static const quint8 MAX_PROCESSORS_PER_TRACK = 4;
protected:

    inline virtual void preFaderProcess(Audio::SamplesBuffer &out){ Q_UNUSED(out) } // called after process all input and plugins, and just before compute gain, pan and boost.

    int getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght);

    QSet<AudioNode *> connections;
    AudioNodeProcessor *processors[MAX_PROCESSORS_PER_TRACK];
    SamplesBuffer internalInputBuffer;
    SamplesBuffer internalOutputBuffer;

    mutable Audio::AudioPeak lastPeak;
    QMutex mutex; // used to protected connections manipulation because nodes can be added or removed by different threads
private:
    AudioNode(const AudioNode &other);
    AudioNode &operator=(const AudioNode &other);

    bool muted;
    bool soloed;

    bool activated;// used when room stream is played. All tracks are disabled, except the room streamer.

    float gain;
    float boost;

    // pan
    float pan;
    float leftGain;
    float rightGain;

    static const double ROOT_2_OVER_2;
    static const double PI_OVER_2;

    double resamplingCorrection;

    void updateGains();

signals:
    void gainChanged(float newGain);
    void panChanged(float newPan);
    // void boostChanged(float boostInDecibels);
    void muteChanged(bool muteStatus);
    void soloChanged(bool soloStatus);
};

}//namespace

#endif
