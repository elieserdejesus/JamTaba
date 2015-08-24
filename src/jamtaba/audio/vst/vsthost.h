#ifndef VSTHOST_H
#define VSTHOST_H

#if _WIN32
    #include "aeffectx.h"
#endif

//#include <QDebug>

namespace Midi {
class MidiBuffer;
}


namespace Vst {

class VstPlugin;



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
    void setTempo(int bpm);
    void setPlayingFlag(bool playing);
    //void setTransportChangedFlag(bool transportChanged);
    void update(int intervalPosition);
protected:
    static VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode, VstInt32 index, VstInt32 value, void *ptr, float opt);
private:
    VstTimeInfo vstTimeInfo;
    int sampleRate;
    int blockSize;

    void clearVstTimeInfoFlags();


    static VstHost* hostInstance;
    VstHost();
    VstHost(const VstHost&);//copy constructor
    ~VstHost();
};

}

#endif // VSTHOST_H

