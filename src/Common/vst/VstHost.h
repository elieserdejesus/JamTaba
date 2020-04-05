#ifndef VSTHOST_H
#define VSTHOST_H

#include "../../VST_SDK/VST2_SDK/public.sdk/source/vst2.x/aeffectx.h"
#include <QScopedPointer>
#include <QObject>
#include "midi/MidiMessage.h"
#include "../audio/Host.h"

namespace vst {

class VstPlugin;
class VstLoader;

class VstHost : public QObject, public Host
{

    Q_OBJECT

    friend class VstPlugin;
    friend class VstLoader;

public:
    static VstHost *getInstance();

    ~VstHost();
    int getSampleRate() const override;
    inline int getBufferSize() const override
    {
        return blockSize;
    }

    std::vector<midi::MidiMessage> pullReceivedMidiMessages() override;

    void setSampleRate(int sampleRate) override;
    void setBlockSize(int blockSize) override;
    void setTempo(int bpm) override;
    void setPlayingFlag(bool playing) override;
    void setPositionInSamples(int intervalPosition) override;

protected:
    static long VSTCALLBACK hostCallback(AEffect *effect, long opcode, long index, long value,
                                         void *ptr, float opt);

signals:
    void pluginRequestingWindowResize(const QString &pluginName, int newWidth, int newHeight);

private:
    VstTimeInfo vstTimeInfo;

    int blockSize;

    void clearVstTimeInfoFlags();

    static QScopedPointer<VstHost> hostInstance;
    VstHost();
    VstHost(const VstHost &); // copy constructor

    bool tempoIsValid() const;

};
}

#endif // VSTHOST_H
