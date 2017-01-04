#ifndef _AUDIO_UNIT_UTILS_H_
#define _AUDIO_UNIT_UTILS_H_

#include "audio/core/Plugins.h"
#include <QString>
#include <QByteArray>

#include <AudioUnit/AudioUnit.h>

namespace AU {

    class AudioUnitPlugin : public Audio::Plugin {

    public:
        AudioUnitPlugin(const QString &name, const QString &path, AudioUnit au, int initialSampleRate, int blockSize);
        virtual ~AudioUnitPlugin();

        void openEditor(const QPoint &centerOfScreen) override;
        void closeEditor() override;

        void start() override;

        inline QByteArray getSerializedData() const override { return QByteArray(); }
        inline void restoreFromSerializedData(const QByteArray &data) override { Q_UNUSED(data) };

        void setSampleRate(int newSampleRate) override;

        void process(const Audio::SamplesBuffer &inBuffer, Audio::SamplesBuffer &outBuffer,
                             const QList<Midi::MidiMessage> &midiBuffer) override;

        void suspend() override;
        void resume() override;
        void updateGui() override;

        inline QString getPath() const override { return path; }

    private:
        QDialog *createEditorWindow(AudioUnit unit);
        AudioUnit audioUnit;
        QString path;

        AudioBufferList *bufferList;


        // AU callbacks
        static OSStatus getInputCallback (void* hostRef, AudioUnitRenderActionFlags* ioActionFlags,
                                                    const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
                                                    UInt32 inNumberFrames, AudioBufferList* ioData);

        static OSStatus getBeatAndTempoCallback (void* hostRef, Float64* outCurrentBeat, Float64* outCurrentTempo);


        static OSStatus getMusicalTimeLocationCallback (void* hostRef, UInt32* outDeltaSampleOffsetToNextBeat,
                                                            Float32* outTimeSig_Numerator, UInt32* outTimeSig_Denominator,
                                                            Float64* outCurrentMeasureDownBeat);

        static OSStatus getTransportStateCallback (void* hostRef, Boolean* outIsPlaying, Boolean* outTransportStateChanged,
                                                       Float64* outCurrentSampleInTimeLine, Boolean* outIsCycling,
                                                       Float64* outCycleStartBeat, Float64* outCycleEndBeat);
    };

    // name space functions
    QString osTypeToString (OSType type);
    OSType stringToOSType (const QString& string);

    bool getComponentDescriptionFromPath(const QString &path, AudioComponentDescription &desc);

    AudioUnitPlugin *audioUnitPluginfromPath(const QString &path, int initialSampleRate, int blockSize);

} // namespace


#endif
