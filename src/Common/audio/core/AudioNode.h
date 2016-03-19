#ifndef AUDIO_NODE_H
#define AUDIO_NODE_H

#include <QSet>
#include <QMutex>
#include "SamplesBuffer.h"
#include "AudioDriver.h"
#include <QDebug>

namespace Midi   {
class MidiBuffer;
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
                                  const Midi::MidiBuffer &midiBuffer);
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

    virtual void addProcessor(AudioNodeProcessor *newProcessor);
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

protected:

    int getInputResamplingLength(int sourceSampleRate, int targetSampleRate, int outFrameLenght);

    QSet<AudioNode *> connections;
    QList<AudioNodeProcessor *> processors;
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
