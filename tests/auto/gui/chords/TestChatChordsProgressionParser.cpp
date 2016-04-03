#include "TestChatChordsProgressionParser.h"
#include <QTest>
#include <QDebug>
#include "gui/chords/ChatChordsProgressionParser.h"

void TestChatChordsProgressionParser::upperCaseText()
{
    //testing if upper case letters are recognized as chords
    QFETCH(QString, chatMessage);
    ChatChordsProgressionParser parser;
    QVERIFY(!parser.containsProgression(chatMessage));
}

void TestChatChordsProgressionParser::upperCaseText_data()
{

    QTest::addColumn<QString>("chatMessage");

    QTest::newRow("Testing issue #275") << QString("I AM TIRED ...");
    QTest::newRow("Testing issue #263") << QString("LETS TAKE A SHIT");

}

void TestChatChordsProgressionParser::validMeasuresCount()
{
    QFETCH(QString, chatMessage);
    QFETCH(int, expectedMeasuresCount);
    ChatChordsProgressionParser parser;
    ChordProgression progression = parser.parse(chatMessage);
    QCOMPARE(progression.getMeasures().size(), expectedMeasuresCount);
}

void TestChatChordsProgressionParser::validMeasuresCount_data()
{
    QTest::addColumn<QString>("chatMessage");
    QTest::addColumn<int>("expectedMeasuresCount");

    QTest::newRow("4 measures") << QString("|C|F||G|F") << 4;
    //QTest::newRow("1 measure") << QString("|C") << 1;
    QTest::newRow("0 measure - invalid progression") << QString("it's not a valid progression") << 0;
    QTest::newRow("16 measures") << QString("|C |Fmaj7 |G7 |Gº |Am7 |Am7/G |F#m7(b5) |Fmaj9 |C |Fmaj7 |G7 |Gº |Am7 |Am7/G |F#m7(b5) |Fmaj9") << 16;
}

void TestChatChordsProgressionParser::invalidMeasuresCount()
{
    QFETCH(QString, chatMessage);
    ChatChordsProgressionParser parser;
    QVERIFY(!parser.containsProgression(chatMessage));
}

void TestChatChordsProgressionParser::invalidMeasuresCount_data()
{
    QTest::addColumn<QString>("chatMessage");

    QTest::newRow("1 measure") << QString("|C");
    QTest::newRow("1 measure and some text") << QString("|Camon");
    QTest::newRow("1 measure and some text II") << QString("I Am testing...");
    QTest::newRow("0 measure") << QString("it's not a valid progression");
}

void TestChatChordsProgressionParser::measureSeparators()
{
    QFETCH(QString, chatMessage);
    ChatChordsProgressionParser parser;
    QVERIFY(parser.containsProgression(chatMessage));
}

void TestChatChordsProgressionParser::measureSeparators_data()
{
    QTest::addColumn<QString>("chatMessage");
    QTest::newRow("Measure separator |") << QString("| C | F | G 7 | F");
    QTest::newRow("Measure separator !") << QString("! C ! F ! G 7 ! F");
    QTest::newRow("Measure separator I") << QString("I C I F I G 7 I F");
    QTest::newRow("Measure separator l") << QString("l C l F l G 7 l F");
    //QTest::newRow("Measure separator L") << QString("L C L F L G 7 L F"); //removed to avoid the problem in issue #263
    QTest::newRow("Mixed measure separators") << QString("|C ! F I G 7 l F");
}
