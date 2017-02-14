#include "TestLooper.h"

#include <QString>
#include "audio/core/SamplesBuffer.h"
#include <QTest>

#include "audio/AudioLooper.h"

using namespace Audio;

void TestLooper::multiBufferTest()
{

    Audio::Looper looper;
    looper.playBufferedSamples(true);

    const uint cycleLenghtInSamples = 6;

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(createBuffer("1,2,3"));
    looper.process(createBuffer("4,5,6"));

    SamplesBuffer out(1, 3);
    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(out);
    checkExpectedValues("1,2,3", out);
    out.zero();
    looper.process(out);
    checkExpectedValues("4,5,6", out);

    out.zero();

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(out);
    checkExpectedValues("0, 0, 0", out);
    //out.zero();
    looper.process(out);
    checkExpectedValues("0, 0, 0", out);


}

void TestLooper::basicTest()
{
    SamplesBuffer samples = createBuffer("1,1,1");

    Audio::Looper looper;
    looper.playBufferedSamples(true);

    const uint cycleLenghtInSamples = 3;

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(samples);

    // in first cycle we have all ZERO in looper buffered layer, the expected result is the same as initial samples
    checkExpectedValues("1,1,1", samples);

    samples.add(createBuffer("1, 1, 1")); // samples is 2,2,2 now

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(samples); // now samples will be summed/mixed with previous samples

    checkExpectedValues("3, 3, 3", samples);

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(samples); // now samples will be summed/mixed with previous samples

    checkExpectedValues("5,5,5", samples);

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(samples); // now samples will be summed/mixed with previous samples

    checkExpectedValues("8,8,8", samples);

}

SamplesBuffer TestLooper::createBuffer(QString comaSeparatedValues)
{
    QStringList values;
    if(!comaSeparatedValues.isEmpty())
        values.append(comaSeparatedValues.split(","));
    SamplesBuffer buffer(1, values.size());
    for (int i = 0; i < values.size(); ++i) {
        buffer.set(0, i, values.at(i).toFloat());
    }
    return buffer;
}

void TestLooper::checkExpectedValues(QString comaSeparatedExpectedValues, const SamplesBuffer &buffer)
{
    QStringList expectedValues;
    if(!comaSeparatedExpectedValues.isEmpty())
        expectedValues.append(comaSeparatedExpectedValues.split(","));

    //QCOMPARE(buffer.getFrameLenght(), expectedValues.size());
    for (int i = 0; i < expectedValues.size(); ++i) {
        QCOMPARE(buffer.get(0, i), expectedValues.at(i).toFloat());
    }
}

