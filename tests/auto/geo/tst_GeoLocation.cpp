#include <QObject>
#include <QString>
#include <QtTest/QtTest>
#include "geo/IpToLocationResolver.h"
#include <QDebug>

using namespace geo;

class TestGeoLocation : public QObject
{
    Q_OBJECT

private slots:
    void stripHtmlTags();
    void stripHtmlTags_data();
};

void TestGeoLocation::stripHtmlTags()
{
    QFETCH(QString, dirty);
    QFETCH(QString, clean);

    Location location(dirty, "BR");

    qDebug() << location.getCountryName();

    QCOMPARE(location.getCountryName(), clean);
}

void TestGeoLocation::stripHtmlTags_data()
{
    QTest::addColumn<QString>("dirty");
    QTest::addColumn<QString>("clean");

    QTest::newRow("Strip html tags #1") << "Brazil <div>testing</div>" << "Brazil ";
    QTest::newRow("Strip html tags #2") << "Country name <>testing<>" << "Country name ";
    QTest::newRow("Strip html tags #3") << "Country name <123>testing<>" << "Country name ";
    QTest::newRow("No Html tags to strip") << "Country name" << "Country name";
}


int main(int argc, char *argv[])
{
    TestGeoLocation test;
    return QTest::qExec(&test, argc, argv);
}

#include "tst_GeoLocation.moc"
