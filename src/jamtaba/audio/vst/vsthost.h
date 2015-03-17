#ifndef VSTHOST_H
#define VSTHOST_H

//#include "aeffect.h"
#include "aeffectx.h"
#include <QDebug>

#define MAX_MIDI_EVENTS 20 //in my tests playing piano I can genenerate just 3 messages per block (256 samples) at maximum

namespace Midi {
class MidiBuffer;
}


namespace Vst {

class VstPlugin;

template <int N>
struct VSTEventBlock
{
    VstInt32 numEvents;
    VstIntPtr reserved;
    VstEvent* events[N];
};

//VstInt32 numEvents;		///< number of Events in array
//VstIntPtr reserved;		///< zero (Reserved for future use)
//VstEvent* events[2];

class VstHost
{
    friend class VstPlugin;

public:
    static VstHost* getInstance();
    inline int getSampleRate() const {return sampleRate;}
    inline int getBufferSize() const {return blockSize;}
    void setSampleRate(int sampleRate);
    void setBlockSize(int blockSize);
    void fillMidiEvents(Midi::MidiBuffer& midiIn);
protected:
    static VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode,
      VstInt32 index, VstInt32 value, void *ptr, float opt);


    const VstEvents* getVstMidiEvents() const;

private:
    int sampleRate;
    int blockSize;
    VstTimeInfo* vstTimeInfo;
    VSTEventBlock<MAX_MIDI_EVENTS> vstMidiEvents;

    static VstHost* hostInstance;
    VstHost();
    ~VstHost();
};

}

#endif // VSTHOST_H

