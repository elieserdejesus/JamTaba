#ifndef VSTHOST_H
#define VSTHOST_H

#include "aeffect.h"
#include "aeffectx.h"
#include <QDebug>

namespace Vst {

class VstPlugin;

class VstHost
{
    friend class VstPlugin;

public:
    static VstHost* getInstance();
    inline int getSampleRate() const {return sampleRate;}
    inline int getBufferSize() const {return blockSize;}
    void setSampleRate(int sampleRate);
    void setBlockSize(int blockSize);
protected:
    static VstIntPtr VSTCALLBACK hostCallback(AEffect *effect, VstInt32 opcode,
      VstInt32 index, VstInt32 value, void *ptr, float opt);
private:
    int sampleRate;
    int blockSize;
    VstTimeInfo* vstTimeInfo;

    static VstHost* hostInstance;
    VstHost();
    ~VstHost();
};

}

#endif // VSTHOST_H

