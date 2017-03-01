#include "TestLooper.h"

#include <QString>
#include "audio/core/SamplesBuffer.h"
#include <QTest>
#include <QtGlobal>

#include "audio/looper/AudioLooper.h"

using namespace Audio;

void TestLooper::playingLockedLayersSequence()
{
    QFETCH(quint8, layers);
    QFETCH(QSet<quint8>, lockedLayers);
    QFETCH(QList<quint8>, expectedCurrentLayers);

    Looper looper;
    looper.setLayers(layers);
    looper.selectLayer(layers - 1); // simulate looper in last layer, and in next interval the layer will be the first
    looper.setMode(Looper::SEQUENCE);
    looper.setOption(Looper::PlayLockedLayers, !lockedLayers.isEmpty());

    // lock layers
    for (int l = 0; l < looper.getLayers(); ++l) {
        if (lockedLayers.contains(l))
            looper.setLayerLockedState(l, true);
    }

    looper.play();

    for (int l = 0; l < expectedCurrentLayers.size(); ++l) {
        looper.startNewCycle(2); // start new cycle and increment layer index
        Q_ASSERT(looper.getCurrentLayerIndex() == expectedCurrentLayers.at(l));
    }
}

void TestLooper::playingLockedLayersSequence_data()
{
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<QSet<quint8>>("lockedLayers");
    QTest::addColumn<QList<quint8>>("expectedCurrentLayers");

    QTest::newRow("4 layers, no locked layers") << quint8(4) << (QSet<quint8>()) << (QList<quint8>() << 0 << 1 << 2 << 3);
    QTest::newRow("4 layers, 1st layer locked, playing locked only") << quint8(4) << (QSet<quint8>() << 0) << (QList<quint8>() << 0 << 0 << 0 << 0);
    QTest::newRow("1 layers, 1st layer locked, playing locked only") << quint8(1) << (QSet<quint8>() << 0) << (QList<quint8>() << 0);
    QTest::newRow("2 layers, All layers locked, playing locked only") << quint8(2) << (QSet<quint8>() << 0 << 1) << (QList<quint8>() << 0 << 1);
    QTest::newRow("2 layers, 1st layer locked, playing locked only") << quint8(2) << (QSet<quint8>() << 0) << (QList<quint8>() << 0 << 0);
    QTest::newRow("2 layers, 2nd layer locked, playing locked only") << quint8(2) << (QSet<quint8>() << 1) << (QList<quint8>() << 1 << 1);
}

void TestLooper::hearingRecordedLayersWhileWaiting()
{
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QString, expectedSamples);

    const quint8 layers = 2;
    bool overdubbing = true;

    Looper looper;
    looper.setLayers(layers);

    //create content in all layers
    looper.setMode(Looper::SEQUENCE);
    looper.toggleRecording();
    for (int l = 0; l < layers; ++l) {
        looper.startNewCycle(2);
        QString value(QString::number(1)); // (1, 1) in all layers
        looper.process(createBuffer(value + ", " + value));
    }
    looper.stop(); // finish recording

    looper.setMode(looperMode); // switch to test looper mode
    looper.setOption(Looper::Overdub, overdubbing);
    looper.setOption(Looper::HearAllLayers, true);
    looper.selectLayer(0);// recording in first layer

    looper.toggleRecording(); // waiting state

    SamplesBuffer out = createBuffer("0, 0");
    looper.process(out);
    checkExpectedValues(expectedSamples, out);
}

void TestLooper::hearingRecordedLayersWhileWaiting_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<QString>("expectedSamples");
    QTest::newRow("Hearing pre record material while Waiting (SELECTED mode)") << Looper::SELECTED_LAYER << QString("1, 1");
    QTest::newRow("Hearing pre record material while Waiting (ALL_LAYERS mode)") << Looper::ALL_LAYERS << QString("2, 2");
    QTest::newRow("Hearing pre record material while Waiting (SEQUENCE mode)") << Looper::SEQUENCE << QString("1, 1");
}

void TestLooper::overdubbing()
{
    QFETCH(Looper::Mode, looperMode);
    QFETCH(quint8, layers);
    QFETCH(quint8, recordingLayer);
    QFETCH(bool, hearAllLayers);
    QFETCH(QString, defaultLayersContent);
    QFETCH(QString, inputSamples);
    QFETCH(QString, expectedFirstOutput);
    QFETCH(QString, expectedSecondOutput);

    Looper looper;
    looper.setLayers(layers);

    //create content in all layers
    looper.setMode(Looper::SEQUENCE);
    looper.toggleRecording();
    for (int l = 0; l < layers; ++l) {
        looper.startNewCycle(2);
        looper.process(createBuffer(defaultLayersContent));
    }
    looper.stop(); // finish recording default layers content

    looper.setMode(looperMode);
    looper.setOption(Looper::Overdub, true); // overdub always ON
    looper.setOption(Looper::HearAllLayers, hearAllLayers);

    looper.selectLayer(recordingLayer);

    looper.toggleRecording(); // waiting state

    looper.startNewCycle(2);
    Q_ASSERT(looper.getCurrentLayerIndex() == recordingLayer);
    SamplesBuffer firstBuffer = createBuffer(inputSamples);
    looper.process(firstBuffer);
    checkExpectedValues(expectedFirstOutput, firstBuffer);

    looper.startNewCycle(2);
    Q_ASSERT(looper.getCurrentLayerIndex() == recordingLayer);
    SamplesBuffer secondBuffer = createBuffer(inputSamples);
    looper.process(secondBuffer);
    checkExpectedValues(expectedSecondOutput, secondBuffer);
}

void TestLooper::overdubbing_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<quint8>("recordingLayer");
    QTest::addColumn<bool>("hearAllLayers");
    QTest::addColumn<QString>("defaultLayersContent");
    QTest::addColumn<QString>("inputSamples");
    QTest::addColumn<QString>("expectedFirstOutput");
    QTest::addColumn<QString>("expectedSecondOutput");

    QTest::newRow("overdubbing in SEQUENCE mode, 4 layers, recording layer = 0, NOT hearing all layers")
            << Looper::SEQUENCE // looperMode
            << quint8(4)        // layers
            << quint8(0)        // recordingLayer
            << false             // hearAllLayers
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("2, 2")  // expectedFirstOutput
            << QString("3, 3"); // expectedSecondOutput

    QTest::newRow("Overdubbing in ALL_LAYERS mode, 4 layers, recording layer = 1")
            << Looper::ALL_LAYERS // looperMode
            << quint8(4)        // layers
            << quint8(1)        // recordingLayer
            << false             // hearAllLayers
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("2, 2")  // expectedFirstOutput
            << QString("3, 3"); // expectedSecondOutput

    QTest::newRow("overdubbing in SELECTED_LAYERS mode, 4 layers, recording layer = 2")
            << Looper::SELECTED_LAYER // looperMode
            << quint8(4)        // layers
            << quint8(2)        // recordingLayer
            << false             // hearAllLayers
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("2, 2")  // expectedFirstOutput
            << QString("3, 3"); // expectedSecondOutput

    QTest::newRow("overdubbing in SEQUENCE mode, 4 layers, recording layer = 0, hearing all layers")
            << Looper::SEQUENCE // looperMode
            << quint8(4)        // layers
            << quint8(0)        // recordingLayer
            << true             // hearAllLayers
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("overdubbing in ALL_LAYERS mode, 4 layers, recording layer = 1, hearing all layers")
            << Looper::ALL_LAYERS // looperMode
            << quint8(4)        // layers
            << quint8(1)        // recordingLayer
            << true             // hearAllLayers
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("overdubbing in SELECTED_LAYERS mode, 4 layers, recording layer = 2, hearing all layers")
            << Looper::SELECTED_LAYER // looperMode
            << quint8(4)        // layers
            << quint8(2)        // recordingLayer
            << true             // hearAllLayers (will have no effect in SELECTED mode)
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("2, 2")  // expectedFirstOutput
            << QString("3, 3"); // expectedSecondOutput

}

void TestLooper::resizeLayersAndCopySamples()
{
    QFETCH(QStringList, initialBuffer);
    QFETCH(QStringList, finalBuffer);

    Looper looper;
    looper.setLayers(1);

    // simulate recording
    looper.toggleRecording();
    looper.startNewCycle(initialBuffer.count());
    looper.process(createBuffer(initialBuffer.join(',')));

    uint newSamplesPerCycle = finalBuffer.count();
    looper.startNewCycle(newSamplesPerCycle); // force recording stop, resize and copy samples

    SamplesBuffer out(1, newSamplesPerCycle);
    looper.process(out);
    checkExpectedValues(finalBuffer.join(','), out);
}

void TestLooper::resizeLayersAndCopySamples_data()
{
    QTest::addColumn<QStringList>("initialBuffer");
    QTest::addColumn<QStringList>("finalBuffer");

    QTest::newRow("3 samples resized to 6") << (QStringList() << "1" << "2" << "3") << (QStringList() << "1" << "2" << "3" << "1" << "2" << "3");
    QTest::newRow("2 samples, resized to 6") << (QStringList() << "1" << "2") << (QStringList() << "1" << "2" << "1" << "2" << "1" << "2");
    QTest::newRow("2 samples, resized to 8") << (QStringList() << "1" << "2") << (QStringList() << "1" << "2" << "1" << "2" << "1" << "2" << "1" << "2");
    QTest::newRow("4 samples, resized to 2") << (QStringList() << "1" << "2" << "3" << "4") << (QStringList() << "1" << "2");
    QTest::newRow("4 samples, resized to 3") << (QStringList() << "1" << "2" << "3" << "4") << (QStringList() << "1" << "2" << "3");
    QTest::newRow("2 samples, resized to 3") << (QStringList() << "1" << "2") << (QStringList() << "1" << "2" << "1");
    QTest::newRow("2 samples, resized to 5") << (QStringList() << "1" << "2") << (QStringList() << "1" << "2" << "1" << "2" << "1");
}

void TestLooper::firstUnlockedLayer()
{
    QFETCH(quint8, maxLayers);
    QFETCH(QList<quint8>, lockedLayers);
    QFETCH(quint8, currentLayer);
    QFETCH(int, expectedRecordingLayer);
    QFETCH(bool, isWaiting);

    Looper looper;
    looper.setLayers(maxLayers);
    looper.selectLayer(currentLayer);

    for (quint8 layer : lockedLayers)
        looper.setLayerLockedState(layer, true);

    looper.toggleRecording(); // activate recording and check the currentLayerIndex

    QVERIFY(looper.getCurrentLayerIndex() == expectedRecordingLayer);
    QVERIFY(looper.isWaiting() == isWaiting);

}

void TestLooper::firstUnlockedLayer_data()
{
    QTest::addColumn<quint8>("maxLayers");
    QTest::addColumn<QList<quint8>>("lockedLayers");
    QTest::addColumn<quint8>("currentLayer");
    QTest::addColumn<int>("expectedRecordingLayer");
    QTest::addColumn<bool>("isWaiting"); //expected looper state

    QTest::newRow("One layer, no lock") << quint8(1) << QList<quint8>() << quint8(0) << int(0) << true;
    QTest::newRow("One layer, first layer locked") << quint8(1) << (QList<quint8>() << 0) << quint8(0) << int(0) << false;

    QTest::newRow("2 layers, second layer locked, current layer=0") << quint8(2) << (QList<quint8>() << 1) << quint8(0) << int(0) << true;
    QTest::newRow("2 layers, second layer locked, current layer=1") << quint8(2) << (QList<quint8>() << 1) << quint8(1) << int(0) << true;
    QTest::newRow("2 layers, first layer locked, current layer=0") << quint8(2) << (QList<quint8>() << 0) << quint8(0) << int(1) << true;

    QTest::newRow("4 layers, no lock, current layer=3") << quint8(4) << QList<quint8>() << quint8(3) << int(3) << true;
    QTest::newRow("4 layers, fist layer locked, current layer=0") << quint8(4) << (QList<quint8>() << 0) << quint8(0) << int(1) << true;
    QTest::newRow("4 layers, fist layer locked, current layer=1") << quint8(4) << (QList<quint8>() << 0) << quint8(1) << int(1) << true;
    QTest::newRow("4 layers, fist layer locked, current layer=2") << quint8(4) << (QList<quint8>() << 0) << quint8(2) << int(2) << true;
    QTest::newRow("4 layers, third layer locked, current layer=0") << quint8(4) << (QList<quint8>() << 2) << quint8(0) << int(0) << true;
    QTest::newRow("4 layers, first 3 layers locked, current layer=2") << quint8(4) << (QList<quint8>() << 0 << 1 << 2) << quint8(2) << int(3) << true;
    QTest::newRow("4 layers, layers 0 and 2 locked, current layer=2") << quint8(4) << (QList<quint8>() << 0 << 2) << quint8(2) << int(3) << true;
    QTest::newRow("4 layers, layers last 3 layers locked, current layer=2") << quint8(4) << (QList<quint8>() << 1 << 2 << 3) << quint8(2) << int(0) << true;

    QTest::newRow("4 layers, all layers locked, can't record") << quint8(4) << (QList<quint8>() << 0 << 1 << 2 << 3) << quint8(0) << int(0) << false;

}

void TestLooper::multiBufferTest()
{

    Audio::Looper looper;
    looper.setLayers(1);
    looper.toggleRecording();

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
    checkExpectedValues("1, 2, 3", out);
    out.zero();
    looper.process(out);
    checkExpectedValues("4, 5, 6", out);


}

void TestLooper::basicTest()
{
    SamplesBuffer samples = createBuffer("1,1,1");

    Audio::Looper looper;
    looper.setLayers(1);
    looper.toggleRecording();

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

    checkExpectedValues("4,4,4", samples);

    looper.startNewCycle(cycleLenghtInSamples);
    looper.process(samples); // now samples will be summed/mixed with previous samples

    checkExpectedValues("5,5,5", samples);

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

