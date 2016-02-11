#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include "audio/core/SamplesBuffer.h"

using namespace Audio;

class TestSamplesBuffer: public QObject
{
    Q_OBJECT

private slots:
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

private:
    SamplesBuffer createBuffer(QString comaSeparatedValues);
    void checkExpectedValues(QString comaSeparatedExpectedValues, const SamplesBuffer &buffer);
};

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

int main(int argc, char *argv[])
{
    TestSamplesBuffer test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_Audio.moc"
