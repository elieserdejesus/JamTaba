
#include <QTest>
#include "EmojiUtil.h"

using namespace EmojiUtil;

class TestEmojiUtil: public QObject
{
    Q_OBJECT

private slots:

    void emojifyNotation();
    void emojifyNotation_data();
};


void TestEmojiUtil::emojifyNotation_data()
{
    QTest::addColumn<QString>("text");
    QTest::addColumn<int>("count");

    QTest::newRow("single simle") << ":smile:" << 1;
    QTest::newRow("double simle") << ":smile::smile:" << 2;
    QTest::newRow("this will not make double simle") << ":smile:smile:" << 1;
    QTest::newRow("space is not identifier") << ":big smile:" << 0;
}


void TestEmojiUtil::emojifyNotation()
{
    QFETCH(QString, text);
    QFETCH(int, count);

    QCOMPARE(emojify(text).count("<img "), count);
}


QTEST_MAIN(TestEmojiUtil)
#include "tst_EmojiUtil.moc"
