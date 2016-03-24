#include <QObject>
#include <QtTest/QtTest>
#include <QString>
#include "midi/MidiMessage.h"

using namespace Midi;

class TestMidiMessage: public QObject
{
    Q_OBJECT

private slots:
    void transpose();
    void transpose_data();
};

void TestMidiMessage::transpose()
{
    QFETCH(quint32, midiMessageData);
    QFETCH(qint8, transpose);

    MidiMessage message(midiMessageData, 0, 0);
    int originalNote = message.getData1();
    message.transpose(transpose);
    QCOMPARE(originalNote + transpose, message.getData1());
}

void TestMidiMessage::transpose_data()
{
    QTest::addColumn<quint32>("midiMessageData");
    QTest::addColumn<qint8>("transpose");

    //Note 80 (0x50), Channel 0, max velocity (127=7F)
    QTest::newRow("Transpose +1") << (quint32)0x7F5090 << (qint8)1;

    //Note 70 (0x46), Channel 0, max velocity (127=7F)
    QTest::newRow("Transpose +12") << (quint32)0x7F4690 << (qint8)12;

    //Note 70 (0x46), Channel 0, max velocity (127=7F)
    QTest::newRow("Transpose -12") << (quint32)0x7F4690 << (qint8)-12;

}

int main(int argc, char *argv[])
{
    TestMidiMessage test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_MidiMessage.moc"
