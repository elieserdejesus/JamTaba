#ifndef _AUDIO_NODE_PROCESSOR_H_
#define _AUDIO_NODE_PROCESSOR_H_

#include <QObject>
#include "midi/MidiMessage.h"


namespace Audio {

class SamplesBuffer;

class AudioNodeProcessor : public QObject  // TODO - this inheritance is really necessary?
{
public:
    AudioNodeProcessor();
    virtual ~AudioNodeProcessor()
    {
    }

    virtual void process(const Audio::SamplesBuffer &in, Audio::SamplesBuffer &out,
                         const QList<Midi::MidiMessage> &midiMessages) = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void updateGui() = 0;
    virtual void openEditor(const QPoint &centerOfScreen) = 0;
    virtual void closeEditor() = 0;
    virtual void setSampleRate(int newSampleRate)
    {
        Q_UNUSED(newSampleRate);
    }

    virtual void setBypass(bool state);
    bool isBypassed() const
    {
        return bypassed;
    }

    virtual bool isVirtualInstrument() const
    {
        return false;
    }

protected:
    bool bypassed;
};

}//namespace

#endif
