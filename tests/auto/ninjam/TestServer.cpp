#include "TestServer.h"
#include "ninjam/client/Server.h"
#include "ninjam/client/User.h"
#include <QTest>

using namespace ninjam;

void TestServer::addUser()
{
    Server server("localhost", 2040, 2);
    QString userFullName("anon@localhost");
    User user(userFullName);
    server.addUser(user);

    //test overloaded versions
    QVERIFY(server.containsUser(userFullName));
    QVERIFY(server.containsUser(user));
}


void TestServer::addUserChannel_data()
{
    QTest::addColumn<quint8>("serverMaxChannels");
    QTest::addColumn<quint8>("channelsToAdd");

    QTest::newRow("Server with 2 max channels, adding 1 channels") << (quint8)2 << (quint8)1;
    QTest::newRow("Server with 2 max channels, adding 2 channels") << (quint8)2 << (quint8)2;
    QTest::newRow("Server with 2 max channels, adding 3 channels") << (quint8)2 << (quint8)3;
}

void TestServer::addUserChannel()
{
    QFETCH(quint8, serverMaxChannels);
    QFETCH(quint8, channelsToAdd);

    Server server("localhost", 2040, serverMaxChannels);
    QString userFullName("anon@localhost");
    server.addUser(User(userFullName));

    //adding some channels
    for (quint8 channelIndex = 0; channelIndex < channelsToAdd; ++channelIndex) {
        QString channelName = "channel " + QString::number(channelIndex);
        server.addUserChannel(UserChannel(userFullName, channelName, channelIndex));
    }

    //checking
    for (quint8 channelIndex = 0; channelIndex < channelsToAdd; ++channelIndex) {
        User user = server.getUser(userFullName);
        bool channelIsExpected = channelsToAdd > 0 && channelIndex < server.getMaxChannels();
        QString expectedChannelName = channelIsExpected ? ("channel " + QString::number(channelIndex)) : "invalid";
        QCOMPARE(user.hasChannel(channelIndex), channelIsExpected);
        QString channelName = user.getChannel(channelIndex).getName();
        QCOMPARE(channelName, expectedChannelName);
    }
}

void TestServer::removeUserChannel_data()
{
    QTest::addColumn<quint8>("channelsToAdd");
    QTest::addColumn<quint8>("channelsToRemove");

    QTest::newRow("Addind 2 and Removing 2 channels") << (quint8)2 << (quint8)2;
    QTest::newRow("Addind 1 and Removing 2 channels") << (quint8)1 << (quint8)2;
}


void TestServer::removeUserChannel()
{
    Server server("localhost", 2040, 2);
    QString userFullName("anon@localhost");
    server.addUser(User(userFullName));

    QFETCH(quint8, channelsToAdd);
    QFETCH(quint8, channelsToRemove);

    //adding some channels
    for (quint8 channelIndex = 0; channelIndex < channelsToAdd; ++channelIndex) {
        QString channelName = "channel " + QString::number(channelIndex);
        server.addUserChannel(UserChannel(userFullName, channelName, channelIndex));
    }

    //removing channels and check
    for (quint8 channelIndex = 0; channelIndex < channelsToRemove; ++channelIndex) {
        UserChannel channel = server.getUser(userFullName).getChannel(channelIndex);
        if(channelsToRemove >= channelsToAdd){
            server.removeUserChannel(channel);
            QVERIFY(!server.getUser(userFullName).hasChannel(channel.getIndex()));
        }
        else{
            QCOMPARE(channel.getName(), QString("invalid"));
        }
    }
}

void TestServer::updateUserChannel_data()
{
    QTest::addColumn<quint8>("channelsToAdd");
    QTest::addColumn<quint8>("channelsToUpdate");

    QTest::newRow("Addind 2 and Updating 2 channels") << (quint8)2 << (quint8)2;
    QTest::newRow("Addind 1 and Updating 2 channels") << (quint8)1 << (quint8)2;
}

void TestServer::updateUserChannel()
{
    Server server("localhost", 2040, 2);
    QString userFullName("anon@localhost");
    server.addUser(User(userFullName));

    QFETCH(quint8, channelsToAdd);
    QFETCH(quint8, channelsToUpdate);

    //adding channels
    for (quint8 index = 0; index < channelsToAdd; ++index) {
        server.addUserChannel(UserChannel(userFullName, "channel name " + QString::number(index), index));
    }

    //updating channels names
    for (quint8 index = 0; index < channelsToUpdate; ++index) {
        bool channelIsExpected = index < channelsToAdd;
        QString newName = "newChannelName" + QString::number(index);
        server.updateUserChannel(UserChannel(userFullName, newName, index));
        QString channelName = server.getUser(userFullName).getChannel(index).getName();
        QCOMPARE(channelName, channelIsExpected ? newName : QString("invalid"));
    }
}

