#ifndef _AUDIO_UNIT_UTILS_H_
#define _AUDIO_UNIT_UTILS_H_

#include <QString>
#include <QByteArray>

#include <QMacCocoaViewContainer>

#include "audio/core/Plugins.h"
#include "audio/core/SamplesBuffer.h"
#include "AU/AudioUnitHost.h"

#include <AudioUnit/AudioUnit.h>
#include "PublicUtility/CABufferList.h"

namespace AU {

    class AudioUnitPlugin : public Audio::Plugin {

    public:
        AudioUnitPlugin(const QString &name, const QString &path, AudioUnit au, AudioUnitHost *host);
        virtual ~AudioUnitPlugin();

        void openEditor(const QPoint &centerOfScreen) override;
        void closeEditor() override;

        void start() override;

        inline QByteArray getSerializedData() const override { return QByteArray(); }
        inline void restoreFromSerializedData(const QByteArray &data) override { Q_UNUSED(data); }

        void setSampleRate(int newSampleRate) override;

        void process(const Audio::SamplesBuffer &inBuffer, Audio::SamplesBuffer &outBuffer,
                             std::vector<Midi::MidiMessage> &midiBuffer) override;

        void suspend() override;
        void resume() override;
        void updateGui() override;

        inline QString getPath() const override { return path; }

        bool isVirtualInstrument() const override;

    private:
        AudioUnit audioUnit;
        QString path;

        AudioBufferList *bufferList;
        const Audio::SamplesBuffer *currentInputBuffer;
        Audio::SamplesBuffer internalOutBuffer;

        const bool hasInputs;
        const bool hasOutputs;

        const bool wantsMidiMessages;

        AudioUnitHost *host;
        AudioTimeStamp timeStamp;

        QMacCocoaViewContainer *viewContainer;

                // AU callbacks
        static OSStatus inputCallback (void* hostRef, AudioUnitRenderActionFlags* ioActionFlags,
                                                    const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
                                                    UInt32 inNumberFrames, AudioBufferList* ioData);

        static OSStatus getBeatAndTempoCallback (void* hostRef, Float64* outCurrentBeat, Float64* outCurrentTempo);


        static OSStatus getMusicalTimeLocationCallback (void* hostRef, UInt32* outDeltaSampleOffsetToNextBeat,
                                                            Float32* outTimeSig_Numerator, UInt32* outTimeSig_Denominator,
                                                            Float64* outCurrentMeasureDownBeat);

        static OSStatus getTransportStateCallback (void* hostRef, Boolean* outIsPlaying, Boolean* outTransportStateChanged,
                                                       Float64* outCurrentSampleInTimeLine, Boolean* outIsCycling,
                                                       Float64* outCycleStartBeat, Float64* outCycleEndBeat);

        UInt32 initializeBuses(AudioUnitScope scope);
        void initializeStreamFormat(AudioUnitScope scope, UInt32 channels, Float64 sampleRate);
        void initializeCallbacks();
        void initializeMaximumFramesPerSlice(UInt32 maxFrames);
        void initializeSampleRate(Float64 initialSampleRate);
        void initializeChannelLayout(AudioUnitScope scope);

        void copyBufferContent(const Audio::SamplesBuffer *input, AudioBufferList *buffer, quint32 frames);

        static bool audioUnitWantsMidi(AudioUnit audioUnit);

        static quint8 getBusCount(AudioUnit audioUnit, AudioUnitScope scope);

    };

    // name space functions
    QString osTypeToString (OSType type);
    OSType stringToOSType (const QString& string);

    bool getComponentDescriptionFromPath(const QString &path, AudioComponentDescription &desc);

    AudioUnitPlugin *audioUnitPluginfromPath(const QString &path, int initialSampleRate, int blockSize);

    Audio::PluginDescriptor createPluginDescriptor(const QString &name, const QString &path);

} // namespace


#endif
