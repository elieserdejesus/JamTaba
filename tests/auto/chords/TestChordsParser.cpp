#include <QTest>
#include <QDebug>
#include "TestChordsParser.h"
#include "gui/chords/ChatChordsProgressionParser.h"

void TestChordsParser::validChords()
{
    QFETCH(QString, chordText);

    QVERIFY(ChatChordsProgressionParser::isValidChord(chordText));
}

void TestChordsParser::validChords_data()
{
    QTest::addColumn<QString>("chordText");

    const char* chords[] = {"Cmaj7", "Cmaj9", "C7+", "C+7", "C+", "CM", "Cm", "Cmin",
                           "C7", "C7(9)", "C7(9 11)", "C7 (9 #11)",
                            "Cmin7", "Cmin9", "Cº", "Cmin7(b5)", "Cmin7(9)", "Cmin(maj7)",
                            "C/Bb", "C#/Bb", "C#/A#", "C#7/A#", "C#7(9)/A#", "C#min7(9)/A#",
                            "Csus", "Cadd2", "C2", "C9"
                           };

    for( const char* chordText : chords )
        QTest::newRow(chordText) << QString(chordText);

}

void TestChordsParser::invalidChords()
{
    QFETCH(QString, chordText);

    QVERIFY(!ChatChordsProgressionParser::isValidChord(chordText));
}

void TestChordsParser::invalidChords_data()
{
    QTest::addColumn<QString>("chordText");

    const char* texts[] = {"test", "not chord", "Ctest"};//, "C123"};

    for( const char* text : texts )
        QTest::newRow(text) << QString(text);

}
