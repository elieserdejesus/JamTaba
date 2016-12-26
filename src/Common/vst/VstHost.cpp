#include "VstHost.h"
#include "aeffectx.h"
#include "midi/MidiDriver.h"
#include <QDebug>
#include <QDateTime>
#include <QApplication>
#include <QList>
#include <QMap>
#include <cmath>
#include "log/Logging.h"

using namespace Vst;

QScopedPointer<VstHost> VstHost::hostInstance;

VstHost *VstHost::getInstance()
{
    if (!hostInstance)
        hostInstance.reset(new VstHost());
    return hostInstance.data();
}

VstHost::VstHost() :
    blockSize(0)
{
    clearVstTimeInfoFlags();
}

void VstHost::clearVstTimeInfoFlags()
{
    int tempSampleRate = vstTimeInfo.sampleRate;

    vstTimeInfo.samplePos = 0.0;
    vstTimeInfo.sampleRate = tempSampleRate;
    vstTimeInfo.nanoSeconds = 0.0;
    vstTimeInfo.ppqPos = 0.0;
    vstTimeInfo.tempo = 120.0;
    vstTimeInfo.barStartPos = 0.0;
    vstTimeInfo.cycleStartPos = 0.0;
    vstTimeInfo.cycleEndPos = 0.0;
    vstTimeInfo.timeSigNumerator = 4;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.smpteOffset = 0;
    vstTimeInfo.smpteFrameRate = 1;
    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.flags = 0;
    vstTimeInfo.flags |= kVstTempoValid;
    vstTimeInfo.flags |= kVstTimeSigValid;
}

void VstHost::setPlayingFlag(bool playing)
{
    if (playing)
        vstTimeInfo.flags |= kVstTransportPlaying;
    else
        clearVstTimeInfoFlags();
}

QList<Midi::MidiMessage> VstHost::pullReceivedMidiMessages()
{
    QList<Midi::MidiMessage> messages(receivedMidiMessages);
    receivedMidiMessages.clear();
    return messages;
}

void VstHost::update(int intervalPosition)
{
    vstTimeInfo.samplePos = intervalPosition;

    int measure = (int)vstTimeInfo.ppqPos/vstTimeInfo.timeSigNumerator;
    vstTimeInfo.barStartPos = measure * vstTimeInfo.timeSigNumerator;

    vstTimeInfo.smpteOffset = measure + 1;

    vstTimeInfo.samplesToNextClock = 0;
    vstTimeInfo.timeSigDenominator = 4;
    vstTimeInfo.timeSigDenominator = 4;

    vstTimeInfo.nanoSeconds = QDateTime::currentDateTime().currentMSecsSinceEpoch() * 1000000.0;

    // ++++++++++++++
    // bar length in quarter notes
    float barLengthq = (float)(4*vstTimeInfo.timeSigNumerator)/vstTimeInfo.timeSigDenominator;

    vstTimeInfo.cycleEndPos = 0; // barLengthq*loopLenght;
    vstTimeInfo.cycleStartPos = 0;

    double dPos = vstTimeInfo.samplePos / vstTimeInfo.sampleRate;
    vstTimeInfo.ppqPos = dPos * vstTimeInfo.tempo / 60.L;

    int currentBar = std::floor(vstTimeInfo.ppqPos/barLengthq);
    vstTimeInfo.barStartPos = barLengthq*currentBar;
    // +++++++

    vstTimeInfo.flags = 0;
    vstTimeInfo.flags |= kVstTransportChanged;// = 1,		///< indicates that play, cycle or record state has changed
    vstTimeInfo.flags |= kVstTransportPlaying;// = 1 << 1,	///< set if Host sequencer is currently playing
    // vstTimeInfo.flags |= kVstTransportCycleActive;// = 1 << 2,	///< set if Host sequencer is in cycle mode
    // vstTimeInfo.flags |= kVstAutomationReading;//    = 1 << 7,	///< set if automation read mode active (play parameter changes)
    vstTimeInfo.flags |= kVstNanosValid;// = 1 << 8,	///< VstTimeInfo::nanoSeconds valid
    vstTimeInfo.flags |= kVstPpqPosValid;// = 1 << 9,	///< VstTimeInfo::ppqPos valid
    vstTimeInfo.flags |= kVstTempoValid;// = 1 << 10,	///< VstTimeInfo::tempo valid
    vstTimeInfo.flags |= kVstBarsValid;// = 1 << 11,	///< VstTimeInfo::barStartPos valid
    // vstTimeInfo.flags |= kVstCyclePosValid;//        = 1 << 12,	///< VstTimeInfo::cycleStartPos and VstTimeInfo::cycleEndPos valid
    vstTimeInfo.flags |= kVstTimeSigValid;// = 1 << 13,	///< VstTimeInfo::timeSigNumerator and VstTimeInfo::timeSigDenominator valid
    // vstTimeInfo.flags |= kVstSmpteValid;//           = 1 << 14,	///< VstTimeInfo::smpteOffset and VstTimeInfo::smpteFrameRate valid
    vstTimeInfo.flags |= kVstClockValid;
}

VstHost::~VstHost()
{
}

void VstHost::setBlockSize(int blockSize)
{
    this->blockSize = blockSize;
}

void VstHost::setSampleRate(int sampleRate)
{
    this->vstTimeInfo.sampleRate = sampleRate;
}

int VstHost::getSampleRate() const
{
    return (int)this->vstTimeInfo.sampleRate;
}

void VstHost::setTempo(int bpm)
{
    this->vstTimeInfo.tempo = bpm;
    this->vstTimeInfo.flags |= kVstTempoValid;
}

bool VstHost::tempoIsValid() const
{
    return this->vstTimeInfo.flags & kVstTempoValid;
}

long VSTCALLBACK VstHost::hostCallback(AEffect *effect, long opcode, long index, long value, void *ptr,
                                    float opt)
{
    Q_UNUSED(effect)
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(opt)

    switch (opcode) {
    case audioMasterIdle:
        QApplication::processEvents();
        return 0L;

    case audioMasterVersion:  // 1
        return 2400L;

    case audioMasterSizeWindow:
    {
        // [index]: new width [value]: new height [return value]: 1 if supported
        int newWidth = index;
        int newHeight = value;
        char temp[128];// kVstMaxEffectNameLen]; //some dumb plugins don't respect kVstMaxEffectNameLen
        effect->dispatcher(effect, effGetEffectName, 0, 0, temp, 0);
        emit VstHost::getInstance()->pluginRequestingWindowResize(QString::fromUtf8(
                                                                   temp), newWidth, newHeight);
        return 1L;
    }

    case audioMasterGetBlockSize:
        return VstHost::getInstance()->blockSize;

    case audioMasterGetSampleRate:
        return VstHost::getInstance()->getSampleRate();

    case audioMasterWantMidi:// 6
        return true;

    case audioMasterGetTime:  // 7
        return (long)(&VstHost::getInstance()->vstTimeInfo);

    case audioMasterGetCurrentProcessLevel:  // 23
        return 2L;

    case audioMasterGetVendorString:  // 32
        strcpy((char *)ptr, "www.jamtaba.com");
        return 1L;

    case audioMasterGetProductString:  // 33
        strcpy((char *)ptr, "Jamtaba II");
        return 1L;

    case audioMasterGetVendorVersion:  // 34
        return 1L;

    case audioMasterProcessEvents:    // 8 - receiving midi events generated by vst plugins
    {
        VstEvents *vstEvents = (VstEvents *)ptr;
        if (vstEvents) {
            for (int i = 0; i < vstEvents->numEvents; ++i) {
                if (vstEvents->events[i]->type == kVstMidiType) {
                    VstMidiEvent *vstMidiEvent = (VstMidiEvent *)vstEvents->events[i];
                    Midi::MidiMessage msg = Midi::MidiMessage::fromArray(vstMidiEvent->midiData);
                    hostInstance->receivedMidiMessages.append(msg);
                }
            }
        }
        return 1L;
    }

    case audioMasterUpdateDisplay:// 42
        QApplication::processEvents();
        return 1L;

    case audioMasterCanDo:  // 37
        const char *str = (const char *)ptr;

        if ((!strcmp(str, "sendVstEvents"))
            || (!strcmp(str, "sendVstMidiEvent"))
            || (!strcmp(str, "receiveVstMidiEvent"))
            || (!strcmp(str, "receiveVstEvents"))
            || (!strcmp(str, "sizeWindow"))
            || (!strcmp(str, "sendVstMidiEventFlagIsRealtime"))
            || (!strcmp(str, "sendVstTimeInfo"))
            || (!strcmp(str, "supplyIdle"))
            )
            return 1L;

        // ignore the rest
        qCDebug(jtStandaloneVstHost) << "host can't do"<<str;
        return 0;
    }
    qCDebug(jtStandaloneVstHost) << "not handled hostCallBack opcode:" << opcode;
    return 0L;
}
