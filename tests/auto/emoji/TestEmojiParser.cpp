#include <QTest>
#include "TestEmojiParser.h"
#include "gui/chat/EmojiManager.h"

void TestEmojiParser::combinationEmojisInsideMessages_data()
{
    QTest::addColumn<QString>("message");
    QTest::addColumn<QString>("emojifiedMessage");

    QTest::newRow(":-o)")  << QString("Nice! :-o) jam!") << QString("Nice! 😲 jam!");
    QTest::newRow(":-O)")  << QString("Nice! :-O) jam!") << QString("Nice! 😲 jam!");

    QTest::newRow(":/)")  << QString("Nice! :/) jam!") << QString("Nice! 😕 jam!");
    QTest::newRow(":-/)")  << QString("Nice! :-/) jam!") << QString("Nice! 😕 jam!");

    QTest::newRow(":)")   << QString("Nice! :) jam!")  << QString("Nice! 😀 jam!");
    QTest::newRow(":-)")  << QString("Nice! :-) jam!") << QString("Nice! 😀 jam!");

    QTest::newRow(":(")  << QString("Nice! :( jam!") << QString("Nice! 😞 jam!");
    QTest::newRow(":-(")  << QString("Nice! :-( jam!") << QString("Nice! 😞 jam!");

    QTest::newRow(";)")  << QString("Nice! ;) jam!") << QString("Nice! 😉 jam!");
    QTest::newRow(";)")  << QString("<-;)") << QString("<-😉");
    QTest::newRow(";)")  << QString("<- ;)") << QString("<- 😉");
    QTest::newRow(";-)")  << QString("Nice! ;-) jam!") << QString("Nice! 😉 jam!");

    QTest::newRow(":p")  << QString("Nice! :p jam!") << QString("Nice! 😛 jam!");
    QTest::newRow(":-p")  << QString("Nice! :-p jam!") << QString("Nice! 😛 jam!");
    QTest::newRow(":P")  << QString("Nice! :P jam!") << QString("Nice! 😛 jam!");
    QTest::newRow(":-P")  << QString("Nice! :-P jam!") << QString("Nice! 😛 jam!");

    QTest::newRow("(y)")  << QString("Nice! (y) jam!") << QString("Nice! 👍 jam!");
    QTest::newRow("(n)")  << QString("Nice! (n) jam!") << QString("Nice! 👎 jam!");
    QTest::newRow("+1")  << QString("Nice! +1 jam!") << QString("Nice! 👍 jam!");
    QTest::newRow("-1")  << QString("Nice! -1 jam!") << QString("Nice! 👎 jam!");

    QTest::newRow(":*")  << QString("Nice! :* jam!") << QString("Nice! 😗 jam!");
    QTest::newRow(":-*")  << QString("Nice! :-* jam!") << QString("Nice! 😗 jam!");

    QTest::newRow(":'(")  << QString("Nice! :'( jam!") << QString("Nice! 😢 jam!");
    QTest::newRow(":'-(")  << QString("Nice! :'-( jam!") << QString("Nice! 😢 jam!");

    QTest::newRow(":D")  << QString("Nice! :D jam!") << QString("Nice! 😃 jam!");
    QTest::newRow(":-D)")  << QString("Nice! :-D) jam!") << QString("Nice! 😃 jam!");

    QTest::newRow(":@")  << QString("Nice! :@ jam!") << QString("Nice! 😠 jam!");
    QTest::newRow(":-@)")  << QString("Nice! :-@) jam!") << QString("Nice! 😠 jam!");
}

void TestEmojiParser::combinationEmojisInsideMessages()
{
    QFETCH(QString, message);
    QFETCH(QString, emojifiedMessage);

    auto manager = new EmojiManager(QString(), QString());

    QString result = manager->emojify(message);
    QCOMPARE(result, emojifiedMessage);
}

void TestEmojiParser::combinationEmojis_data()
{
    QTest::addColumn<QString>("message");
    QTest::addColumn<QString>("emojifiedMessage");

    QTest::newRow(":-o)")  << QString(":-o)") << QString("😲");
    QTest::newRow(":-O)")  << QString(":-O)") << QString("😲");

    QTest::newRow(":/)")  << QString(":/)") << QString("😕");
    QTest::newRow(":-/)")  << QString(":-/)") << QString("😕");

    QTest::newRow(":)")   << QString(":)")  << QString("😀");
    QTest::newRow(":-)")  << QString(":-)") << QString("😀");

    QTest::newRow(":(")  << QString(":(") << QString("😞");
    QTest::newRow(":-(")  << QString(":-(") << QString("😞");

    QTest::newRow(";)")  << QString(";)") << QString("😉");
    QTest::newRow(";-)")  << QString(";-)") << QString("😉");

    QTest::newRow(":p")  << QString(":p") << QString("😛");
    QTest::newRow(":-p")  << QString(":-p") << QString("😛");
    QTest::newRow(":P")  << QString(":P") << QString("😛");
    QTest::newRow(":-P")  << QString(":-P") << QString("😛");

    QTest::newRow("(y)")  << QString("(y)") << QString("👍");
    QTest::newRow("(n)")  << QString("(n)") << QString("👎");
    QTest::newRow("+1")  << QString("+1") << QString("👍");
    QTest::newRow("-1")  << QString("-1") << QString("👎");

    QTest::newRow(":*")  << QString(":*") << QString("😗");
    QTest::newRow(":-*")  << QString(":-*") << QString("😗");

    QTest::newRow(":'(")  << QString(":'(") << QString("😢");
    QTest::newRow(":'-(")  << QString(":'-(") << QString("😢");

    QTest::newRow(":D")  << QString(":D") << QString("😃");
    QTest::newRow(":-D)")  << QString(":-D)") << QString("😃");

    QTest::newRow(":@")  << QString(":@") << QString("😠");
    QTest::newRow(":-@)")  << QString(":-@)") << QString("😠");
}

void TestEmojiParser::combinationEmojis()
{
    QFETCH(QString, message);
    QFETCH(QString, emojifiedMessage);

    auto manager = new EmojiManager(QString(), QString());

    QString result = manager->emojify(message);
    QCOMPARE(result, emojifiedMessage);
}
