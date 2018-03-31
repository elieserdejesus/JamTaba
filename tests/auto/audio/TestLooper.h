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

    void recordAndHearingIncommingAudioThroughLooperLayerSettings();
    void recordAndHearingIncommingAudioThroughLooperLayerSettings_data();

    void playing();
    void playing_data();

    void hearLockedLayersOnlyAfterRecord(); // first problem in issue #823
    void monitoringWhenPlayLockedAndHearAllAreChecked(); // second problem in issue #823

private:
    audio::SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const audio::SamplesBuffer &buffer);
};

Q_DECLARE_METATYPE(audio::Looper::RecordingOption)

#endif // TESTLOOPER_H
