#ifndef TESTSAMPLESBUFFER_H
#define TESTSAMPLESBUFFER_H

#include <QObject>
#include "audio/core/SamplesBuffer.h"

class TestSamplesBuffer: public QObject
{
    Q_OBJECT

private slots:
    void invertStereo();
    void invertStereo_data();

    void revertStereo(); // invert stereo, invert again and check if is the same
    void revertStereo_data();

    void append();
    void append_data();

    void discard();
    void discard_data();

    void set_data();
    void set();

    void setFrameLenght_data();
    void setFrameLenght();

    //check if the samples are preserved after the setFrameLengh() is called to increse or decrease the buffer size
    void setFrameLenghtIsPreservingSamples();
    void setFrameLenghtIsPreservingSamples_data();

    void copy();
    void copy_data();

private:
    audio::SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const audio::SamplesBuffer &buffer);
};

#endif // TESTSAMPLESBUFFER_H
