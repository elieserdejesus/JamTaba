#include "TestSamplesBuffer.h"

#include <QString>
#include "audio/core/SamplesBuffer.h"
#include <QTest>

using namespace audio;

void TestSamplesBuffer::copy()
{
    QFETCH(QString, samples); //coma separated sample values

    auto array = createBuffer(samples);
    checkExpectedValues(samples, array);

    SamplesBuffer copy(1);

    copy = array;
    checkExpectedValues(samples, copy);

}

void TestSamplesBuffer::copy_data()
{
    QTest::addColumn<QString>("samples");

    QTest::newRow("1 2 3") << "1,2,3";

}

SamplesBuffer TestSamplesBuffer::createBuffer(QString comaSeparatedValues)
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

void TestSamplesBuffer::checkExpectedValues(QString comaSeparatedExpectedValues, const SamplesBuffer &buffer)
{
    QStringList expectedValues;
    if(!comaSeparatedExpectedValues.isEmpty())
        expectedValues.append(comaSeparatedExpectedValues.split(","));

    //QCOMPARE(buffer.getFrameLenght(), expectedValues.size());
    for (int i = 0; i < expectedValues.size(); ++i) {
        QCOMPARE(buffer.get(0, i), expectedValues.at(i).toFloat());
    }
}

void TestSamplesBuffer::setFrameLenghtIsPreservingSamples()
{
    QFETCH(QString, initialSamples);
    QFETCH(int, firstLenght);
    QFETCH(int, secondLenght);
    QFETCH(QString, firstExpectedSamples);
    QFETCH(QString, secondExpectedSamples);

    SamplesBuffer buffer = createBuffer(initialSamples);

    buffer.setFrameLenght(firstLenght);
    checkExpectedValues(firstExpectedSamples, buffer);

    buffer.setFrameLenght(secondLenght);
    checkExpectedValues(secondExpectedSamples, buffer);
}

void TestSamplesBuffer::setFrameLenghtIsPreservingSamples_data()
{
    QTest::addColumn<QString>("initialSamples");
    QTest::addColumn<int>("firstLenght");
    QTest::addColumn<int>("secondLenght");
    QTest::addColumn<QString>("firstExpectedSamples");
    QTest::addColumn<QString>("secondExpectedSamples");

    QTest::newRow("Samples are preserved after Grow and Shrunk")
            << "1,2,3" << 5 << 3 << "1,2,3,0,0" << "1,2,3";

    QTest::newRow("Samples are preserved after Shrunk and Grow")
            << "1,2,3" << 1 << 3 << "1" << "1,2,3";
}


void TestSamplesBuffer::revertStereo()
{
    QFETCH(QString, leftSamples); //coma separated sample values
    QFETCH(QString, rightSamples);

    SamplesBuffer leftBuffer = createBuffer(leftSamples);
    SamplesBuffer rightBuffer = createBuffer(rightSamples);

    QCOMPARE(leftBuffer.getFrameLenght(), rightBuffer.getFrameLenght()); //checking size

    int size = leftBuffer.getFrameLenght();
    SamplesBuffer stereoBuffer(2);// 2 channels
    stereoBuffer.setFrameLenght(leftBuffer.getFrameLenght());
    for (int sample = 0; sample < size; ++sample) {
        stereoBuffer.set(0, sample, leftBuffer.get(0, sample));
        stereoBuffer.set(1, sample, rightBuffer.get(0, sample));
    }

    stereoBuffer.invertStereo();
    stereoBuffer.invertStereo(); //revert

    for (int sample = 0; sample < size; ++sample) {
        QCOMPARE(stereoBuffer.get(0, sample), leftBuffer.get(0, sample)); // compare left channel from stereoBuffer with leftBuffer (mono buffer)
        QCOMPARE(stereoBuffer.get(1, sample), rightBuffer.get(0, sample)); // compare right channel from stereoBuffer with rightBuffer (mono buffer)
    }

}

void TestSamplesBuffer::revertStereo_data()
{
    QTest::addColumn<QString>("leftSamples");
    QTest::addColumn<QString>("rightSamples");

    QTest::newRow("One sample LR reversion") << "1" << "-1";
    QTest::newRow("3 samples LR reversion") << "1,2,3" << "4,5,6";
}


void TestSamplesBuffer::invertStereo()
{
    QFETCH(QString, leftSamples); //coma separated sample values
    QFETCH(QString, rightSamples);

    SamplesBuffer leftBuffer = createBuffer(leftSamples);
    SamplesBuffer rightBuffer = createBuffer(rightSamples);

    QCOMPARE(leftBuffer.getFrameLenght(), rightBuffer.getFrameLenght()); //checking size

    int size = leftBuffer.getFrameLenght();
    SamplesBuffer stereoBuffer(2);// 2 channels
    stereoBuffer.setFrameLenght(leftBuffer.getFrameLenght());
    for (int sample = 0; sample < size; ++sample) {
        stereoBuffer.set(0, sample, leftBuffer.get(0, sample));
        stereoBuffer.set(1, sample, rightBuffer.get(0, sample));
    }

    stereoBuffer.invertStereo();

    for (int sample = 0; sample < size; ++sample) {
        QCOMPARE(stereoBuffer.get(0, sample), rightBuffer.get(0, sample)); // compare left channel from stereoBuffer with rightBuffer (mono buffer)
        QCOMPARE(stereoBuffer.get(1, sample), leftBuffer.get(0, sample)); // compare right channel from stereoBuffer with leftBuffer (mono buffer)
    }

}

void TestSamplesBuffer::invertStereo_data()
{
    QTest::addColumn<QString>("leftSamples");
    QTest::addColumn<QString>("rightSamples");

    QTest::newRow("One sample LR inversion") << "1" << "-1";
    QTest::newRow("3 samples LR inversion") << "1,2,3" << "4,5,6";
}

void TestSamplesBuffer::setFrameLenght()
{
    QFETCH(QString, initialSamples);
    QFETCH(int, newFrameLenght);
    QFETCH(QString, expectedSamples);

    SamplesBuffer buffer = createBuffer(initialSamples);
    buffer.setFrameLenght(newFrameLenght);
    checkExpectedValues(expectedSamples, buffer);
}

void TestSamplesBuffer::setFrameLenght_data()
{
    QTest::addColumn<QString>("initialSamples");
    QTest::addColumn<int>("newFrameLenght");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("Keep the same lenght") << "1,2,3" << 3 << "1,2,3";
    QTest::newRow("Growing") << "1,2,3" << 5 << "1,2,3,0,0";
    QTest::newRow("Shrunking") << "1,2,3" << 2 << "1,2";
}


void TestSamplesBuffer::set()
{
    QFETCH(QString, initialSamples);
    QFETCH(QString, samplesToSet);
    QFETCH(QString, expectedSamples);

    SamplesBuffer buffer = createBuffer(initialSamples);

    SamplesBuffer bufferToSet = createBuffer(samplesToSet);
    buffer.set(bufferToSet);

    checkExpectedValues(expectedSamples, buffer);
}

void TestSamplesBuffer::set_data()
{
    QTest::addColumn<QString>("initialSamples");
    QTest::addColumn<QString>("samplesToSet");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("Buffers with same size") << "1,2,3" << "4,5,6" << "4,5,6";
    QTest::newRow("Setting a small buffer") << "1,2,3" << "4,5" << "4,5,3";
    QTest::newRow("Setting a large buffer") << "1,2,3" << "4,5,6,7" << "4,5,6";
}

void TestSamplesBuffer::discard()
{
    QFETCH(QString, initialSamples);
    QFETCH(int, samplesToDiscard);
    QFETCH(QString, expectedSamples);

    SamplesBuffer buffer = createBuffer(initialSamples);
    buffer.discardFirstSamples(samplesToDiscard);

    checkExpectedValues(expectedSamples, buffer);
}

void TestSamplesBuffer::discard_data()
{
    QTest::addColumn<QString>("initialSamples");
    QTest::addColumn<int>("samplesToDiscard");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("Discard one sample") << "1,2,3" << 1 << "2,3";
    QTest::newRow("Discard zero samples") << "1,2,3" << 0 << "1,2,3";
    QTest::newRow("Discard all samples") << "1,2,3" << 3 << "";
    QTest::newRow("Discard 2 samples") << "1,2,3" << 2 << "3";
}


void TestSamplesBuffer::append()
{
    QFETCH(QString, initialSamples);
    QFETCH(QString, samplesToAppend);
    QFETCH(QString, expectedSamples);

    SamplesBuffer buffer = createBuffer(initialSamples);

    SamplesBuffer bufferToSet = createBuffer(samplesToAppend);
    buffer.append(bufferToSet);

    checkExpectedValues(expectedSamples, buffer);
}

void TestSamplesBuffer::append_data()
{
    QTest::addColumn<QString>("initialSamples");
    QTest::addColumn<QString>("samplesToAppend");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("Appending 1 sample") << "1,2,3" << "4" << "1,2,3,4";
    QTest::newRow("Appending 2 samples") << "1,2,3" << "4,5" << "1,2,3,4,5";
    QTest::newRow("Appending zero samples") << "1,2,3" << "" << "1,2,3";
}
