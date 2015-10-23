#ifndef VSTHOST_H
#define VSTHOST_H

#include "aeffectx.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(vstHost)

namespace Midi {
class MidiBuffer;
}

namespace Vst {

class VstPlugin;

class Host
{
    friend class VstPlugin;

public:
    static Host* getInstance();
    ~Host();
    int getSampleRate() const;// {return sampleRate;}
    inline int getBufferSize() const {return blockSize;}
    void setSampleRate(int sampleRate);
    void setBlockSize(int blockSize);
    void setTempo(int bpm);
    void setPlayingFlag(bool playing);
    //void setTransportChangedFlag(bool transportChanged);
    void update(int intervalPosition);
protected:
    static long VSTCALLBACK hostCallback(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
private:
    VstTimeInfo vstTimeInfo;
    //int sampleRate;
    int blockSize;

    void clearVstTimeInfoFlags();


    static QScopedPointer<Host> hostInstance;
    Host();
    Host(const Host&);//copy constructor


    bool tempoIsValid() const;

    long callBack(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
};

}

#endif // VSTHOST_H

