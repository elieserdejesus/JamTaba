#include <QObject>
#include <QString>
#include <QtTest/QtTest>
#include "file/FileUtils.h"

class TestFile: public QObject
{
    Q_OBJECT

private slots:
    void sanitizeFileName();
    void sanitizeFileName_data();
};

void TestFile::sanitizeFileName()
{
    QFETCH(QString, original);
    QFETCH(QString, sanitized);

    QCOMPARE(file::sanitizeFileName(original), sanitized);
}

void TestFile::sanitizeFileName_data()
{
    QTest::addColumn<QString>("original");
    QTest::addColumn<QString>("sanitized");

    QTest::newRow("Testing backward slash \\") << "testing \\.json" << "testing _.json";
    QTest::newRow("Testing forward slash /") << "testing /.json" << "testing _.json";
    QTest::newRow("Testing dots :") << "testing :.json" << "testing _.json";
    QTest::newRow("Testing asterisk *") << "testing *.json" << "testing _.json";
    QTest::newRow("Testing question mark ?") << "testing ?.json" << "testing _.json";
    QTest::newRow("Testing quotes \"") << "testing \".json" << "testing _.json";
    QTest::newRow("Testing less than <") << "testing <.json" << "testing _.json";
    QTest::newRow("Testing greater than >") << "testing >.json" << "testing _.json";
    QTest::newRow("Testing pipe |") << "testing |.json" << "testing _.json";
    QTest::newRow("Testing all not allowed symbols") << "testing |\\/?*<>:.json" << "testing ________.json";

}


int main(int argc, char *argv[])
{
    TestFile test;
    return QTest::qExec(&test, argc, argv);
}

#include "test_File.moc"
