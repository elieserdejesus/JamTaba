#ifndef _AUDIO_NODE_PROCESSOR_H_
#define _AUDIO_NODE_PROCESSOR_H_

#include <QObject>
#include "midi/MidiMessage.h"


namespace audio {

class SamplesBuffer;

class AudioNodeProcessor : public QObject  // TODO - this inheritance is really necessary?
{

public:
    AudioNodeProcessor();

    virtual ~AudioNodeProcessor();

    virtual void process(const SamplesBuffer &in, SamplesBuffer &out, std::vector<midi::MidiMessage> &midiMessages) = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void updateGui() = 0;
    virtual void openEditor(const QPoint &centerOfScreen) = 0;
    virtual void closeEditor() = 0;
    virtual void setSampleRate(int newSampleRate);

    virtual void setBypass(bool state);
    bool isBypassed() const;

    virtual bool isVirtualInstrument() const;

    virtual bool canGenerateMidiMessages() const;

protected:
    bool bypassed;

};


inline bool AudioNodeProcessor::isBypassed() const
{
    return bypassed;
}

inline bool AudioNodeProcessor::isVirtualInstrument() const
{
    return false;
}

inline bool AudioNodeProcessor::canGenerateMidiMessages() const
{
    return false;
}

inline AudioNodeProcessor::~AudioNodeProcessor()
{
    //
}

inline void AudioNodeProcessor::setSampleRate(int newSampleRate)
{
    Q_UNUSED(newSampleRate);
}


}//namespace

#endif
