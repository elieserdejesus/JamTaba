#ifndef TESTLOOPER_H
#define TESTLOOPER_H

#include <QObject>
#include "audio/core/SamplesBuffer.h"
#include "audio/looper/AudioLooper.h"

class TestLooper: public QObject
{
    Q_OBJECT

private slots:
    void basicTest();
    void multiBufferTest();

    void firstUnlockedLayer();
    void firstUnlockedLayer_data();

    void resizeLayersAndCopySamples();
    void resizeLayersAndCopySamples_data();

    void overdubbing();
    void overdubbing_data();

    void hearingRecordedLayersWhileWaiting();
    void hearingRecordedLayersWhileWaiting_data();

    void playingLockedLayersSequence();
    void playingLockedLayersSequence_data();

private:
    Audio::SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const Audio::SamplesBuffer &buffer);
};

Q_DECLARE_METATYPE(Audio::Looper::RecordingOption)

#endif // TESTLOOPER_H
