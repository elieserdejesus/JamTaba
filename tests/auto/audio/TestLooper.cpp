#include "TestLooper.h"

#include <QString>
#include "audio/core/SamplesBuffer.h"
#include <QTest>
#include <QtGlobal>

#include "audio/looper/AudioLooper.h"

using namespace Audio;

void TestLooper::autoPlayAfterRecording()
{
    QFETCH(quint8, layers);
    QFETCH(quint8, recLayer);
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QList<quint8>, lockedLayers);
    QFETCH(QList<quint8>, expectedRecLayers);

    Looper looper;
    looper.setLayers(layers);
    looper.selectLayer(recLayer);
    looper.setMode(looperMode);
    looper.setOption(Looper::Overdub, false);

    for (quint8 lockedLayer : lockedLayers)
        looper.setLayerLockedState(lockedLayer, true);

    looper.toggleRecording(); // waiting state

    for (quint8 i = 0; i < expectedRecLayers.size(); ++i) {
        looper.startNewCycle(2);
        QCOMPARE(looper.getCurrentLayerIndex(), expectedRecLayers.at(i));
    }
    looper.startNewCycle(2); // start a new cycle to stop recording

    QVERIFY(looper.isPlaying());
}

void TestLooper::autoPlayAfterRecording_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<quint8>("recLayer");
    QTest::addColumn<QList<quint8>>("lockedLayers");
    QTest::addColumn<QList<quint8>>("expectedRecLayers");

    QTest::newRow("SEQUENCE, 4 layers, no locked layers, rec layer=0")
            << Looper::SEQUENCE       // looper mode
            << quint8(4)              // layers
            << quint8(0)       // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 0 << 1 << 2 << 3);     // expected rec layers

    QTest::newRow("SEQUENCE, 4 layers, no locked layers, rec layer=2")
            << Looper::SEQUENCE       // looper mode
            << quint8(4)              // layers
            << quint8(2)       // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 2 << 3);     // expected rec layers

    QTest::newRow("SEQUENCE, 4 layers, no locked layers, rec layer=3")
            << Looper::SEQUENCE       // looper mode
            << quint8(4)              // layers
            << quint8(3)       // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 3);     // expected rec layers


    QTest::newRow("SEQUENCE, 4 layers, first layer locked, rec layer=0")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 0)           // locked layers
            << (QList<quint8>() << 1 << 2 << 3);// expected recording layers

    QTest::newRow("SEQUENCE, 4 layers, 2nd layer locked, rec layer=0")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 1)           // locked layers
            << (QList<quint8>() << 0 << 2 << 3);// expected recording layers

    QTest::newRow("SEQUENCE, 4 layers, 3rd layer locked, rec layer=0")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 2)           // locked layers
            << (QList<quint8>() << 0 << 1 << 3);// expected recording layers

    QTest::newRow("SEQUENCE, 4 layers, First 2 layers locked, rec layer=0")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 0 << 1)      // locked layers
            << (QList<quint8>() << 2 << 3);// expected recording layers

    QTest::newRow("SEQUENCE, 4 layers, Last 2 layers locked, rec layer=0")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 2 << 3)      // locked layers
            << (QList<quint8>() << 0 << 1);// expected recording layers

    QTest::newRow("SEQUENCE, 4 layers, Last 2 layers locked, rec layer=3")
            << Looper::SEQUENCE                 // looper mode
            << quint8(4)                        // layers
            << quint8(3)                        // rec layer
            << (QList<quint8>() << 2 << 3)      // locked layers
            << (QList<quint8>() << 0 << 1);// expected recording layers

}

void TestLooper::invalidRecordingStart()
{
    QFETCH(quint8, layers);
    QFETCH(quint8, recLayer);
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QList<quint8>, lockedLayers);

    Looper looper;
    looper.setLayers(layers);
    looper.selectLayer(recLayer);
    looper.setMode(looperMode);

    for (quint8 lockedLayer : lockedLayers)
        looper.setLayerLockedState(lockedLayer, true);

    looper.toggleRecording(); // try rec in invalid state

    QVERIFY(looper.isStopped());
}

void TestLooper::invalidRecordingStart_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<quint8>("recLayer");
    QTest::addColumn<QList<quint8>>("lockedLayers");

    QTest::newRow("SEQUENCE, 4 layers, rec layer=0, all layers locked")
            << Looper::SEQUENCE                         // looper mode
            << quint8(4)                                // layers
            << quint8(0)                                // rec layer
            << (QList<quint8>() << 0 << 1 << 2 << 3);    // locked layers
}

void TestLooper::playing()
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

void TestLooper::playing_data()
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

void TestLooper::waiting()
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

void TestLooper::waiting_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<QString>("expectedSamples");
    QTest::newRow("Hearing pre record material while Waiting (SELECTED mode)") << Looper::SELECTED_LAYER << QString("1, 1");
    QTest::newRow("Hearing pre record material while Waiting (ALL_LAYERS mode)") << Looper::ALL_LAYERS << QString("2, 2");
    QTest::newRow("Hearing pre record material while Waiting (SEQUENCE mode)") << Looper::SEQUENCE << QString("1, 1");
}

void TestLooper::recording()
{
    QFETCH(Looper::Mode, looperMode);
    QFETCH(quint8, layers);
    QFETCH(QList<quint8>, recordingLayers);
    QFETCH(bool, hearAllLayers);
    QFETCH(bool, overdubbing);
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
    looper.setOption(Looper::Overdub, overdubbing);
    looper.setOption(Looper::HearAllLayers, hearAllLayers);

    looper.selectLayer(recordingLayers.first());

    looper.toggleRecording(); // waiting state

    QString expectedOutputs[] = {expectedFirstOutput, expectedSecondOutput};

    for (int i = 0; i < 2; ++i) {
        looper.startNewCycle(2);
        Q_ASSERT(looper.getCurrentLayerIndex() == recordingLayers.at(i));
        SamplesBuffer buffer = createBuffer(inputSamples);
        looper.process(buffer);
        checkExpectedValues(expectedOutputs[i], buffer);
    }
}

void TestLooper::recording_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<QList<quint8>>("recordingLayers");
    QTest::addColumn<bool>("hearAllLayers");
    QTest::addColumn<bool>("overdubbing");
    QTest::addColumn<QString>("defaultLayersContent");
    QTest::addColumn<QString>("inputSamples");
    QTest::addColumn<QString>("expectedFirstOutput");
    QTest::addColumn<QString>("expectedSecondOutput");

    Looper::Mode modes[] = {Looper::SEQUENCE, Looper::SELECTED_LAYER};
    for (int layer = 0; layer < 4; ++layer) {
        for (Looper::Mode mode : modes) {
            QString title(Looper::getModeString(mode) + " mode, 4 layers, rec layer( " + QString::number(layer) + "), Overdubbing");
            QTest::newRow(title.toUtf8().data())
                    << mode // looperMode
                    << quint8(4)        // layers
                    << (QList<quint8>() << layer << layer)        // recordingLayer
                    << false            // hearAllLayers
                    << true             // overdubbing
                    << QString("1, 1")  // defaultLayersContent
                    << QString("1, 1")  // inputSamples
                    << QString("2, 2")  // expectedFirstOutput
                    << QString("3, 3"); // expectedSecondOutput
        }
    }

    QTest::newRow("SEQUENCE mode, 4 layers, rec layer = 0, overdubbing, hearing all")
            << Looper::SEQUENCE // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 0 << 0)        // recordingLayer
            << true             // hearAllLayers
            << true             // overdubbing
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("SEQUENCE mode, 4 layers, rec layer = 0")
            << Looper::SEQUENCE // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 0 << 1)        // recordingLayer
            << false            // hearAllLayers
            << false            // overdubbing
            << QString("0, 0")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("1, 1")  // expectedFirstOutput
            << QString("1, 1"); // expectedSecondOutput


    QTest::newRow("ALL_LAYERS mode, 4 layers, rec layer = 1, overdubbing, hearing all")
            << Looper::ALL_LAYERS // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 1 << 1)        // recordingLayers
            << true             // hearAllLayers (always ON in ALL_LAYERS mode)
            << true             // overdubbing
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("ALL_LAYERS mode, 4 layers, rec layer = 0, hearing all (always ON in this mode)")
            << Looper::ALL_LAYERS       // looperMode
            << quint8(4)                // layers
            << (QList<quint8>() << 0 << 1)   // recordingLayers
            << true                     // hearAllLayers (always ON in ALL_LAYERS mode)
            << false                    // overdubbing
            << QString("1, 1")          // defaultLayersContent
            << QString("1, 1")          // inputSamples
            << QString("4, 4")          // expectedFirstOutput
            << QString("4, 4");         // expectedSecondOutput

    QTest::newRow("SELECTED_LAYERS mode, 4 layers, rec layer = 2, overdubbing, hear all")
            << Looper::SELECTED_LAYER // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 2 << 2)        // recordingLayers
            << true             // hearAllLayers (will have no effect in SELECTED mode)
            << true             // overdubbing
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

