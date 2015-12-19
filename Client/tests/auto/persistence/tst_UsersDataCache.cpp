
#include <QObject>
#include <QString>
#include <QtTest/QtTest>
#include "persistence/UsersDataCache.h"

class TestCacheEntry: public QObject
{
	Q_OBJECT

private slots:
	void validUserIp_data();
	void validUserIp();
	void invalidUserIp_data();
	void invalidUserIp();

	void validUserName_data();
	void validUserName();
	void invalidUserName_data();
	void invalidUserName();
};

void TestCacheEntry::validUserIp_data()
{
    QTest::addColumn<QString>("addr");
    QTest::newRow("local address") << "127.0.0.1";
    QTest::newRow("local address with mask") << "127.0.0.x";

    // this is invalid as IPv4 address out of 8bit range
    // but we does not need to check. not making a connection.
    QTest::newRow("invalid but welformed") << "234.456.678.890";
}

void TestCacheEntry::validUserIp()
{
    QFETCH(QString, addr);
    QVERIFY(Persistence::CacheEntry::ipPattern.exactMatch(addr));
}

void TestCacheEntry::invalidUserIp_data()
{
	
}

void TestCacheEntry::invalidUserIp()
{
	
}

void TestCacheEntry::validUserName_data()
{
	
}

void TestCacheEntry::validUserName()
{
	
}

void TestCacheEntry::invalidUserName_data()
{
	
}

void TestCacheEntry::invalidUserName()
{
	
}


int main(int argc, char *argv[])
{
	int status = 0;

	{
		TestCacheEntry test;
		status |= QTest::qExec(&test, argc, argv);
	}

    return status;
}

#include "tst_UsersDataCache.moc"