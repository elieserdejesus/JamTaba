#ifndef TESTLOOPER_H
#define TESTLOOPER_H

#include <QObject>
#include "audio/core/SamplesBuffer.h"

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

private:
    Audio::SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const Audio::SamplesBuffer &buffer);
};

#endif // TESTLOOPER_H
