#ifndef VSTHOST_H
#define VSTHOST_H

#include "aeffectx.h"
#include <QScopedPointer>
#include <QObject>
#include "midi/MidiMessage.h"

namespace Vst {

class VstPlugin;
class VstLoader;

class Host : public QObject
{
    Q_OBJECT

    friend class VstPlugin;
    friend class VstLoader;

public:
    static Host *getInstance();

    ~Host();
    int getSampleRate() const;
    inline int getBufferSize() const
    {
        return blockSize;
    }

    QList<Midi::MidiMessage> getReceivedMidiMessages() const;
    void clearReceivedMidiMessages();

    void setSampleRate(int sampleRate);
    void setBlockSize(int blockSize);
    void setTempo(int bpm);
    void setPlayingFlag(bool playing);
    void update(int intervalPosition);
protected:
    static long VSTCALLBACK hostCallback(AEffect *effect, long opcode, long index, long value,
                                         void *ptr, float opt);

signals:
    void pluginRequestingWindowResize(const QString &pluginName, int newWidth, int newHeight);

private:
    VstTimeInfo vstTimeInfo;
    QList<Midi::MidiMessage> receivedMidiMessages;

    int blockSize;

    void clearVstTimeInfoFlags();

    static QScopedPointer<Host> hostInstance;
    Host();
    Host(const Host &);// copy constructor

    bool tempoIsValid() const;

    long callBack(AEffect *effect, long opcode, long index, long value, void *ptr, float opt);
};
}

#endif // VSTHOST_H
