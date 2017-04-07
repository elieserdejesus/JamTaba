#ifndef TESTLOOPER_H
#define TESTLOOPER_H

#include <QObject>
#include "audio/core/SamplesBuffer.h"
#include "looper/Looper.h"

class TestLooper: public QObject
{
    Q_OBJECT

private slots:

    void multiBufferTest();

    void firstUnlockedLayer();
    void firstUnlockedLayer_data();

    void resizeLayersAndCopySamples();
    void resizeLayersAndCopySamples_data();

    void recording();
    void recording_data();

    void autoPlayAfterRecording();
    void autoPlayAfterRecording_data();

    void invalidRecordingStart();
    void invalidRecordingStart_data();

    void waitingToRecordAndHearingPreRecordedMaterial();
    void waitingToRecordAndHearingPreRecordedMaterial_data();

    void waitingToRecordAndPreserveRecordedMaterialWhenSkipRecording();
    void waitingToRecordAndPreserveRecordedMaterialWhenSkipRecording_data();

    void waitingToRecordAndHearingIncommingAudioThroughLooperLayerSettings();
    void waitingToRecordAndHearingIncommingAudioThroughLooperLayerSettings_data();

    void playing();
    void playing_data();

private:
    Audio::SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const Audio::SamplesBuffer &buffer);
};

Q_DECLARE_METATYPE(Audio::Looper::RecordingOption)

#endif // TESTLOOPER_H
