#ifndef VSTHOST_H
#define VSTHOST_H

#include "aeffectx.h"
#include <QScopedPointer>
#include <QObject>

namespace Midi {
class MidiBuffer;
}

namespace Vst {

class VstPlugin;
class VstLoader;

class Host : public QObject
{
    Q_OBJECT

    friend class VstPlugin;
    friend class VstLoader;

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

signals:
    void pluginRequestingWindowResize(QString pluginName, int newWidth, int newHeight);

private:
    VstTimeInfo vstTimeInfo;

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

