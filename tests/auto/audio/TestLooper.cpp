#include "TestLooper.h"

#include <QString>
#include "audio/core/SamplesBuffer.h"
#include <QTest>
#include <QtGlobal>

#include "looper/Looper.h"

using namespace audio;

void TestLooper::monitoringWhenPlayLockedAndHearAllAreChecked() // testing second problem described in #823
{
    const uint cycleLenght = 2;
    const uint layers = 2;

    Looper looper;
    looper.setLayers(layers, true);

    Looper::Mode modes[] = {Looper::Sequence, Looper::SelectedLayer, Looper::AllLayers};

    for (auto mode : modes) {
        looper.setMode(mode);

        // 'checking' the checkboxes
        looper.setOption(Looper::PlayLockedLayers, true);
        looper.setOption(Looper::HearAllLayers, true);

        looper.setLayerLockedState(0, true); // locking first layer

        // simulate pre-recorded material
        looper.startNewCycle(cycleLenght);
        for (uint l = 0; l < layers; ++l) {
            QString value = QString::number(l+1);
            looper.setLayerSamples(l, createBuffer(value + ", " + value));
            looper.setLayerPan(l, -1); // avoiding pan law in expected values
        }

        looper.selectLayer(1); // second layer, first layer is locked
        looper.clearCurrentLayer();

        // waiting to record
        looper.toggleRecording();
        Q_ASSERT(looper.isWaitingToRecord());

        // recording
        looper.startNewCycle(cycleLenght);
        Q_ASSERT(looper.isRecording());
        QVERIFY(looper.getCurrentLayerIndex() == 1);

        // check if monitoring is correct
        SamplesBuffer samples = createBuffer("2, 2");
        looper.addBuffer(samples); // recording '2, 2'

        looper.mixToBuffer(samples);

        checkExpectedValues("3, 3", samples);

        looper.startNewCycle(cycleLenght); // stop
    }
}

void TestLooper::hearLockedLayersOnlyAfterRecord()
{
    // testing first problem described in #823

    const uint layers = 3;
    const uint cycleLenght = 2;

    Looper looper;
    looper.setLayers(layers, true);
    looper.setMode(Looper::Sequence);

    // simulate pre-recorded material in first 3 layers
    looper.startNewCycle(cycleLenght);
    for (uint l = 0; l < layers; ++l) {
        QString value = QString::number(l+1);
        looper.setLayerSamples(l, createBuffer(value + ", " + value));
        looper.setLayerPan(l, -1); // avoiding pan law in expected values
    }

    // lock the 2 first layers
    looper.setLayerLockedState(0, true);
    looper.setLayerLockedState(1, true);

    // 'checking' the "play locked" checkbox
    looper.setOption(Looper::PlayLockedLayers, true);

    // focusing in unlocked layer
    looper.selectLayer(layers - 1); // focusing the last layer

    // recording
    looper.toggleRecording();
    Q_ASSERT(looper.isWaitingToRecord());

    looper.startNewCycle(cycleLenght);
    Q_ASSERT(looper.isRecording());
    QVERIFY(looper.getCurrentLayerIndex() == 2);

    looper.addBuffer(createBuffer("1, 1"));

    // auto stop recording when start new cycle
    looper.startNewCycle(cycleLenght);
    Q_ASSERT(looper.isPlaying());

    // check if the first layer content (the first locked layer) will be played correctly
    QCOMPARE(looper.getCurrentLayerIndex(), static_cast<quint8>(0)); // check if the looper is playing the first locked layer (first layer)

    SamplesBuffer out(1, cycleLenght);
    looper.mixToBuffer(out);
    checkExpectedValues("1, 1", out); // checking if the first locked layer is rendered correctly

    looper.startNewCycle(cycleLenght); // go to next (2nd) locked layer

    out.zero();
    looper.mixToBuffer(out);
    checkExpectedValues("2, 2", out); // checking if the second locked layer is rendered correctly

    // checking if the next layer will be the first (because we playing locked layers only)
    out.zero();
    looper.startNewCycle(cycleLenght); // now the current layer is 0
    QVERIFY(looper.getCurrentLayerIndex() == static_cast<quint8>(0));
    looper.mixToBuffer(out);
    checkExpectedValues("1, 1", out);
}

void TestLooper::waitingToRecordAndPreserveRecordedMaterialWhenSkipRecording()
{
    QFETCH(QString, preRecordedSamples);
    QFETCH(bool, overdubbing);

    const quint8 layers = 1;

    Looper looper;
    looper.setLayers(layers, true);
    looper.setMode(Looper::SelectedLayer);
    looper.setOption(Looper::Overdub, overdubbing);
    looper.setOption(Looper::HearAllLayers, true);

    for (int l = 0; l < layers; ++l) {
        looper.setLayerPan(l, -1); // avoiding pan law in expected values
    }

    //create pre recorded content
    looper.startNewCycle(2);
    looper.setLayerSamples(0, createBuffer(preRecordedSamples));

    checkExpectedValues(preRecordedSamples, looper.getLayersSamples().first());

    looper.selectLayer(0);
    looper.toggleRecording(); // waiting state
    Q_ASSERT(looper.isWaitingToRecord());

    looper.addBuffer(createBuffer(("0, 0"))); // always input silence to test if pre recorded material will be erased

    SamplesBuffer out(2, 2);
    looper.mixToBuffer(out);

    looper.toggleRecording(); // abort recording

    looper.startNewCycle(2);
    looper.togglePlay();

    checkExpectedValues(preRecordedSamples, out);
}

void TestLooper::waitingToRecordAndPreserveRecordedMaterialWhenSkipRecording_data()
{
    QTest::addColumn<QString>("preRecordedSamples");
    QTest::addColumn<bool>("overdubbing");

    QTest::newRow("Test overdubbing = false") << QString("1, 1") << false;
    //QTest::newRow("Test overdubbing = true") << QString("1, 1") << true;

}

void TestLooper::autoPlayAfterRecording()
{
    QFETCH(quint8, layers);
    QFETCH(quint8, recLayer);
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QList<quint8>, lockedLayers);
    QFETCH(QList<quint8>, expectedRecLayers);

    Looper looper;
    looper.setLayers(layers, true);
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

    QTest::newRow("Sequence, 4 layers, no locked layers, rec layer=0")
            << Looper::Sequence       // looper mode
            << quint8(4)              // layers
            << quint8(0)              // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 0 << 1 << 2 << 3);     // expected rec layers

    QTest::newRow("Sequence, 4 layers, no locked layers, rec layer=2")
            << Looper::Sequence       // looper mode
            << quint8(4)              // layers
            << quint8(2)              // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 2 << 3);     // expected rec layers

    QTest::newRow("Sequence, 4 layers, no locked layers, rec layer=3")
            << Looper::Sequence       // looper mode
            << quint8(4)              // layers
            << quint8(3)              // rec layer
            << (QList<quint8>())      // locked layers
            << (QList<quint8>() << 3);     // expected rec layers


    QTest::newRow("Sequence, 4 layers, first layer locked, rec layer=0")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 0)           // locked layers
            << (QList<quint8>() << 1 << 2 << 3);// expected recording layers

    QTest::newRow("Sequence, 4 layers, first layer locked, rec layer=2")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(2)                        // rec layer
            << (QList<quint8>() << 0)           // locked layers
            << (QList<quint8>() << 2 << 3);// expected recording layers

    QTest::newRow("AllLayers, 4 layers, first layer locked, rec layer=1")
            << Looper::AllLayers                 // looper mode
            << quint8(4)                        // layers
            << quint8(1)                        // rec layer
            << (QList<quint8>() << 0)           // locked layers
            << (QList<quint8>() << 1 << 2 << 3);// expected recording layers

    QTest::newRow("Sequence, 4 layers, 2nd layer locked, rec layer=0")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 1)           // locked layers
            << (QList<quint8>() << 0 << 2 << 3);// expected recording layers

    QTest::newRow("Sequence, 4 layers, 3rd layer locked, rec layer=0")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 2)           // locked layers
            << (QList<quint8>() << 0 << 1 << 3);// expected recording layers

    QTest::newRow("Sequence, 4 layers, First 2 layers locked, rec layer=0")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 0 << 1)      // locked layers
            << (QList<quint8>() << 2 << 3);// expected recording layers

    QTest::newRow("Sequence, 4 layers, Last 2 layers locked, rec layer=0")
            << Looper::Sequence                 // looper mode
            << quint8(4)                        // layers
            << quint8(0)                        // rec layer
            << (QList<quint8>() << 2 << 3)      // locked layers
            << (QList<quint8>() << 0 << 1);// expected recording layers

    QTest::newRow("Sequence, 4 layers, Last 2 layers locked, rec layer=3")
            << Looper::Sequence                 // looper mode
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
    looper.setLayers(layers, true);
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

    QTest::newRow("Sequence, 4 layers, rec layer=0, all layers locked")
            << Looper::Sequence                         // looper mode
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
    looper.setLayers(layers, true);
    looper.selectLayer(layers - 1); // simulate looper in last layer, and in next interval the layer will be the first
    looper.setMode(Looper::Sequence);
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

void TestLooper::recordAndHearingIncommingAudioThroughLooperLayerSettings()
{
    QFETCH(QString, incommingSamples);
    QFETCH(float, recLayerGain);
    QFETCH(quint8, recLayer);
    QFETCH(quint8, layers);
    QFETCH(QString, expectedSamples);
    QFETCH(bool, hearingAllLayers);
    QFETCH(bool, playingLockedLayers);
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QList<quint8>, lockedLayers);

    Looper looper;
    looper.setMode(looperMode);
    looper.setLayers(layers, true);

    for (uint l = 0; l < layers; ++l)
        looper.setLayerPan(l, -1); // avoiding pan law in expected values

    QVERIFY(recLayer < layers);
    QVERIFY(lockedLayers.size() <= layers);

    looper.setLayerGain(recLayer, recLayerGain);

    looper.setOption(Looper::HearAllLayers, hearingAllLayers);
    looper.setOption(Looper::PlayLockedLayers, playingLockedLayers);

    for (quint8 lockedLayer : lockedLayers)
        looper.setLayerLockedState(lockedLayer, true);

    looper.selectLayer(recLayer);

    looper.startNewCycle(incommingSamples.size());
    looper.toggleRecording(); // waiting to record
    QVERIFY(looper.isWaitingToRecord());

    // recording
    looper.startNewCycle(incommingSamples.size());
    Q_ASSERT(looper.isRecording());

    SamplesBuffer buffer = createBuffer(incommingSamples);
    looper.addBuffer(buffer);   // rec
    looper.mixToBuffer(buffer); // render

    checkExpectedValues(expectedSamples, buffer);
}

void TestLooper::recordAndHearingIncommingAudioThroughLooperLayerSettings_data()
{
    QTest::addColumn<QString>("incommingSamples");
    QTest::addColumn<float>("recLayerGain");
    QTest::addColumn<quint8>("recLayer");
    QTest::addColumn<quint8>("layers");
    QTest::addColumn<QString>("expectedSamples");
    QTest::addColumn<bool>("hearingAllLayers");
    QTest::addColumn<bool>("playingLockedLayers");
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<QList<quint8>>("lockedLayers");

    QTest::newRow("50% gain, Hearing all layers, playing locked, Sequence mode, no locked layers")
            << QString("2, 2")  // incomming samples
            << 0.5f             // rec layer gain
            << quint8(0)        // rec layer
            << quint8(1)        // layers
            << QString("1, 1")  // expected samples
            << true             // hearingAllLayers
            << true             // playingLockedLayers
            << Looper::Sequence
            << QList<quint8>(); // locked layers

    QTest::newRow("50% gain, Hearing all layers, playing locked, Selected Layer mode, first layer locked")
            << QString("4, 4")  // incomming samples
            << 0.5f             // layer gain
            << quint8(1)        // rec layer
            << quint8(2)        // layers
            << QString("2, 2")  // expected samples
            << true             // hearingAllLayers
            << true             // playingLockedLayers
            << Looper::SelectedLayer
            << (QList<quint8>() << 0); // locked layers

    QTest::newRow("50% gain, Playing locked, ALL Layers mode, first layer locked")
            << QString("4, 4")  // incomming samples
            << 0.5f             // layer gain
            << quint8(1)        // rec layer
            << quint8(2)        // layers
            << QString("2, 2")  // expected samples
            << false             // hearingAllLayers
            << true             // playingLockedLayers
            << Looper::AllLayers
            << (QList<quint8>() << 0); // locked layers
}

void TestLooper::waitingToRecordAndHearingIncommingAudioThroughLooperLayerSettings()
{
    QFETCH(QString, incommingSamples);
    QFETCH(float, layerGain);
    QFETCH(QString, expectedSamples);

    Looper looper;
    looper.setMode(Looper::SelectedLayer);
    looper.setLayers(1, true);
    looper.setLayerPan(0, -1); // avoiding pan law in expected values

    looper.setLayerGain(0, layerGain);

    looper.startNewCycle(incommingSamples.size());
    looper.toggleRecording(); // waiting to record
    Q_ASSERT(looper.isWaitingToRecord());

    looper.addBuffer(createBuffer(incommingSamples));

    SamplesBuffer out(2, expectedSamples.size());
    looper.mixToBuffer(out);

    checkExpectedValues(expectedSamples, out);
}

void TestLooper::waitingToRecordAndHearingIncommingAudioThroughLooperLayerSettings_data()
{
    QTest::addColumn<QString>("incommingSamples");
    QTest::addColumn<float>("layerGain");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("50% gain I")
            << QString("2, 2") // incomming samples
            << 0.5f  // layer gain
            << QString("1, 1"); // expected samples

    QTest::newRow("50% gain II")
            << QString("4, 4") // incomming samples
            << 0.5f  // layer gain
            << QString("2, 2"); // expected samples
}

void TestLooper::waitingToRecordAndHearingPreRecordedMaterial()
{
    QFETCH(Looper::Mode, looperMode);
    QFETCH(QString, expectedSamples);

    const quint8 layers = 2;
    bool overdubbing = true;

    Looper looper;
    looper.setLayers(layers, true);

    //create content in all layers
    looper.setMode(Looper::Sequence);
    looper.toggleRecording();
    for (int l = 0; l < layers; ++l) {
        looper.setLayerPan(l, -1); // avoiding pan law in expected values
        looper.startNewCycle(2);
        Q_ASSERT(looper.isRecording());
        QString value(QString::number(1)); // (1, 1) in all layers
        looper.addBuffer(createBuffer(value + ", " + value));
    }
    looper.stop(); // finish recording
    Q_ASSERT(looper.isStopped());

    looper.setMode(looperMode); // switch to test looper mode
    looper.setOption(Looper::Overdub, overdubbing);
    looper.setOption(Looper::HearAllLayers, true);
    looper.selectLayer(0);// recording in first layer

    looper.toggleRecording(); // waiting state
    Q_ASSERT(looper.isWaitingToRecord());

    SamplesBuffer out = createBuffer("0, 0");
    looper.mixToBuffer(out);
    checkExpectedValues(expectedSamples, out);
}

void TestLooper::waitingToRecordAndHearingPreRecordedMaterial_data()
{
    QTest::addColumn<Looper::Mode>("looperMode");
    QTest::addColumn<QString>("expectedSamples");

    QTest::newRow("Hearing pre record material while Waiting (SELECTED mode)")
            << Looper::SelectedLayer
            << QString("2, 2");

    QTest::newRow("Hearing pre record material while Waiting (ALL_LAYERS mode)")
            << Looper::AllLayers
            << QString("2, 2");

    QTest::newRow("Hearing pre record material while Waiting (Sequence mode)")
            << Looper::Sequence
            << QString("2, 2"); // hear is activated by default in this test
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
    looper.setLayers(layers, true);
    for (uint l = 0; l < layers; ++l)
        looper.setLayerPan(l, -1); // 100% left to not apply pan law in expected values

    //create content in all layers
    looper.setMode(Looper::Sequence);
    looper.toggleRecording();
    for (int l = 0; l < layers; ++l) {
        looper.startNewCycle(2);
        looper.addBuffer(createBuffer(defaultLayersContent));
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
        looper.addBuffer(buffer);
        looper.mixToBuffer(buffer);
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

    Looper::Mode modes[] = {Looper::Sequence, Looper::SelectedLayer};
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

    QTest::newRow("Sequence mode, 4 layers, rec layer = 0, overdubbing, hearing all")
            << Looper::Sequence // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 0 << 0)        // recordingLayer
            << true             // hearAllLayers
            << true             // overdubbing
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("Sequence mode, 4 layers, rec layer = 0")
            << Looper::Sequence // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 0 << 1)        // recordingLayer
            << false            // hearAllLayers
            << false            // overdubbing
            << QString("0, 0")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("1, 1")  // expectedFirstOutput
            << QString("1, 1"); // expectedSecondOutput


    QTest::newRow("ALL_LAYERS mode, 4 layers, rec layer = 1, overdubbing, hearing all")
            << Looper::AllLayers // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 1 << 1)        // recordingLayers
            << true             // hearAllLayers (always ON in ALL_LAYERS mode)
            << true             // overdubbing
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

    QTest::newRow("ALL_LAYERS mode, 4 layers, rec layer = 0, hearing all (always ON in this mode)")
            << Looper::AllLayers       // looperMode
            << quint8(4)                // layers
            << (QList<quint8>() << 0 << 1)   // recordingLayers
            << true                     // hearAllLayers (always ON in ALL_LAYERS mode)
            << false                    // overdubbing
            << QString("1, 1")          // defaultLayersContent
            << QString("1, 1")          // inputSamples
            << QString("4, 4")          // expectedFirstOutput
            << QString("4, 4");         // expectedSecondOutput

    QTest::newRow("SELECTED_LAYERS mode, 4 layers, rec layer = 2, overdubbing, hear all")
            << Looper::SelectedLayer // looperMode
            << quint8(4)        // layers
            << (QList<quint8>() << 2 << 2)        // recordingLayers
            << true             // hearAllLayers
            << true             // overdubbing
            << QString("1, 1")  // defaultLayersContent
            << QString("1, 1")  // inputSamples
            << QString("5, 5")  // expectedFirstOutput
            << QString("6, 6"); // expectedSecondOutput

}

void TestLooper::resizeLayersAndCopySamples()
{
    QFETCH(QStringList, initialBuffer);
    QFETCH(QStringList, finalBuffer);

    Looper looper;
    looper.setLayers(1, true);

    // simulate recording
    looper.toggleRecording();
    looper.startNewCycle(initialBuffer.count());
    Q_ASSERT(looper.isRecording());
    looper.addBuffer(createBuffer(initialBuffer.join(',')));

    uint newSamplesPerCycle = finalBuffer.count();
    looper.startNewCycle(newSamplesPerCycle); // force recording stop, resize and copy samples

    SamplesBuffer out(1, newSamplesPerCycle);
    looper.setLayerPan(0, -1); // 100% left to not apply pan law in expected values
    looper.mixToBuffer(out);
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
    looper.setMode(Looper::SelectedLayer);
    looper.setLayers(maxLayers, true);
    looper.selectLayer(currentLayer);

    for (quint8 layer : lockedLayers)
        looper.setLayerLockedState(layer, true);

    looper.toggleRecording(); // activate recording and check the currentLayerIndex

    QVERIFY(looper.getCurrentLayerIndex() == expectedRecordingLayer);
    QVERIFY(looper.isWaitingToRecord() == isWaiting);

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

    audio::Looper looper;
    looper.setLayers(1, true);

    looper.toggleRecording();

    const uint cycleLenghtInSamples = 6;

    looper.startNewCycle(cycleLenghtInSamples);
    Q_ASSERT(looper.isRecording());

    looper.setLayerPan(0, -1); // 100% left to not apply pan law in expected values
    looper.addBuffer(createBuffer("1,2,3"));
    auto outBuffer = SamplesBuffer(1, 3);
    looper.mixToBuffer(outBuffer);

    looper.addBuffer(createBuffer("4,5,6"));
    outBuffer = SamplesBuffer(1, 3);
    looper.mixToBuffer(outBuffer);

    SamplesBuffer out(1, 3);
    looper.startNewCycle(cycleLenghtInSamples);

    Q_ASSERT(looper.isPlaying());

    looper.mixToBuffer(out);
    checkExpectedValues("1,2,3", out);
    out.zero();
    looper.mixToBuffer(out);
    checkExpectedValues("4,5,6", out);

    out.zero();

    looper.startNewCycle(cycleLenghtInSamples);
    looper.mixToBuffer(out);
    checkExpectedValues("1, 2, 3", out);
    out.zero();
    looper.mixToBuffer(out);
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

