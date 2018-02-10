#include <QTest>
#include <QDebug>
#include <QDataStream>
#include <QBuffer>
#include <QUuid>
#include "TestMessagesSerialization.h"
#include "ninjam/client/User.h"
#include "ninjam/client/UserChannel.h"
#include "ninjam/Ninjam.h"
#include "ninjam/client/ClientMessages.h"
#include "ninjam/client/ServerMessages.h"

using namespace ninjam;

void TestMessagesSerialization::chatMessage_data(){
    //chat messages always receive a command string and 4 args from server, but some args can be empty strings
    QTest::addColumn<QString>("command");
    QTest::addColumn<QString>("arg1");
    QTest::addColumn<QString>("arg2");
    QTest::addColumn<QString>("arg3");
    QTest::addColumn<QString>("arg4");

    /** From wahjam documentation:
     The server-to-client commands are:
     MSG <username> <text> -- a broadcast message
     PRIVMSG <username> <text> -- a private message
     TOPIC <username> <text> -- server topic change
     JOIN <username> -- user enters server
     PART <username> -- user leaves server
     USERCOUNT <users> <maxusers> -- server status
     */

    QTest::newRow("Broadcast message") << QString("MSG") << QString("sender") <<  QString("message") << QString("") << QString("");
    QTest::newRow("Broadcast message with emoji") << QString("MSG") << QString("message sender name") <<  QString("message content 😀") << QString("") << QString("");
    QTest::newRow("Topic message") << QString("TOPIC") << QString("message sender name") <<  QString("Topic message content") << QString("") << QString("");
    QTest::newRow("Topic including special symbols") << QString("TOPIC") << QString() <<  QString("😀") << QString() << QString();
    QTest::newRow("Joint message") << QString("JOIN") << QString("user name") <<  QString("") << QString("") << QString("");
    QTest::newRow("Part message") << QString("PART") << QString("user name") <<  QString("") << QString("") << QString("");
    QTest::newRow("User count message") << QString("USERCOUNT") << QString("4") <<  QString("8") << QString("") << QString("");
}

void TestMessagesSerialization::chatMessage()
{
    QFETCH(QString, command);
    QFETCH(QString, arg1);
    QFETCH(QString, arg2);
    QFETCH(QString, arg3);
    QFETCH(QString, arg4);

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    ServerToClientChatMessage message(command, arg1, arg2, arg3, arg4);

    message.to(&device);

    device.reset();

    quint32 payload = command.size() + arg1.toUtf8().size() + arg2.toUtf8().size() + arg3.toUtf8().size() + arg4.toUtf8().size();
    payload += 5; // nul terminated strings


    auto header = MessageHeader::from(&device);
    QCOMPARE(header.getPayload(), payload);

    auto otherMessage = ServerToClientChatMessage::from(&device, payload); //populate the message

    QCOMPARE(otherMessage.getCommand(), message.getCommand());
    QCOMPARE(otherMessage.getMessageType(), message.getMessageType());
    QCOMPARE(otherMessage.getArguments(), message.getArguments());
    QCOMPARE(otherMessage.getMessageType(), message.getMessageType());
}

void TestMessagesSerialization::downloadIntervalWrite_data()
{
    QTest::addColumn<quint8>("flags");

    QTest::newRow("download completed") << (quint8)1;
    QTest::newRow("download not completed") << (quint8)0;
}

void TestMessagesSerialization::downloadIntervalWrite()
{
    const char* encodedAudioData = "someFakeOggVorbisBytesJustToTest";

    quint32 payload = static_cast<quint32>(16 + 1 + strlen(encodedAudioData));

    QByteArray GUID(QUuid().toRfc4122());
    QCOMPARE(GUID.size(), 16); //we need a 16 bytes GUID

    QFETCH(quint8, flags);

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    DownloadIntervalWrite msg(GUID, flags, encodedAudioData);
    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);

    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = DownloadIntervalWrite::from(&device, header.getPayload());

    QCOMPARE(msg.getEncodedData(), QByteArray(encodedAudioData));
    QCOMPARE(msg.downloadIsComplete(), flags == 1);
    QCOMPARE(msg.getGUID(), GUID);

    QCOMPARE(msg.getEncodedData(), otherMsg.getEncodedData());
    QCOMPARE(msg.downloadIsComplete(), otherMsg.downloadIsComplete());
    QCOMPARE(msg.getGUID(), otherMsg.getGUID());
    QCOMPARE(msg.getMessageType(), otherMsg.getMessageType());
}

void TestMessagesSerialization::downloadIntervalBegin()
{
    QByteArray GUID(QUuid().toRfc4122());
    QCOMPARE(GUID.size(), 16); //we need a 18 bytes GUID

    quint32 estimatedSize = 0;
    QByteArray fourCC("OGGv");
    quint8 channelIndex = 0;
    const char *userName = "testUserName\0";

    quint32 payload = 16 + 4 + 4 + 1 + qstrlen(userName) + 1;

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    auto msg = DownloadIntervalBegin(GUID, estimatedSize, fourCC, channelIndex, userName);
    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);
    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = DownloadIntervalBegin::from(&device, payload);

    QCOMPARE(msg.getChannelIndex(), channelIndex);
    QCOMPARE(msg.getEstimatedSize(), estimatedSize);
    QCOMPARE(msg.getGUID(), GUID);
    QCOMPARE(msg.getUserName(), QString(userName));

    QCOMPARE(msg.getChannelIndex(), otherMsg.getChannelIndex());
    QCOMPARE(msg.getEstimatedSize(), otherMsg.getEstimatedSize());
    QCOMPARE(msg.getGUID(), otherMsg.getGUID());
    QCOMPARE(msg.getUserName(), otherMsg.getUserName());
    QCOMPARE(msg.getMessageType(), otherMsg.getMessageType());
}

void TestMessagesSerialization::authChallengeMessage_data()
{
    QTest::addColumn<QString>("licenceText");
    QTest::addColumn<quint8>("keepAlivePeriod");

    QTest::newRow("using licence, 30s for keep alive period") << QString("Testing licence\0") << (quint8)30;
    QTest::newRow("using licence, 5s for keep alive period") << QString("Another licence\0") << (quint8)5;
    QTest::newRow("No licence, 15s for keep alive period") << QString("") << (quint8)15;
}

void TestMessagesSerialization::authChallengeMessage()
{
    //build the message raw data
    QByteArray challenge(8, Qt::Uninitialized);
    for (int c = 0; c < challenge.size(); ++c) {
        challenge[c] = c + 48;
    }
    QFETCH(quint8, keepAlivePeriod);
    QFETCH(QString, licenceText);

    quint32 serverCapabilities = keepAlivePeriod << 8;//keep alive period value is stored in bytes 8-15
    if(!licenceText.isEmpty())
        serverCapabilities |= 1;//when server has licence the first bit is set.

    quint32 protocolVersion = 0x00020000;//fixed value
    quint32 payload = 8 + 4 + 4 + licenceText.toUtf8().size() + 1;

    AuthChallengeMessage msg(challenge, licenceText, serverCapabilities, protocolVersion);

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);

    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = AuthChallengeMessage::from(&device, header.getPayload());

    QCOMPARE(otherMsg.getServerKeepAlivePeriod(), serverCapabilities >> 8);
    QCOMPARE(otherMsg.getLicenceAgreement(), licenceText);
    QCOMPARE(otherMsg.getProtocolVersion(), protocolVersion);
    QCOMPARE(otherMsg.getChallenge(), challenge);
    QCOMPARE(otherMsg.serverHasLicenceAgreement(), static_cast<bool>(serverCapabilities & 1));

    QCOMPARE(otherMsg.getServerKeepAlivePeriod(), msg.getServerKeepAlivePeriod());
    QCOMPARE(otherMsg.getLicenceAgreement(), msg.getLicenceAgreement());
    QCOMPARE(otherMsg.getProtocolVersion(), msg.getProtocolVersion());
    QCOMPARE(otherMsg.getChallenge(), msg.getChallenge());
    QCOMPARE(otherMsg.serverHasLicenceAgreement(), msg.serverHasLicenceAgreement());
    QCOMPARE(otherMsg.getMessageType(), msg.getMessageType());
}


void TestMessagesSerialization::authReplyMessage_data()
{
    QTest::addColumn<bool>("userAuthenticated");

    QTest::newRow("User authenticated") << true;
    QTest::newRow("User not authenticated") << false;
}

void TestMessagesSerialization::authReplyMessage()
{
    QFETCH(bool, userAuthenticated);

    // build the message raw data
    quint8 flag = static_cast<quint8>(userAuthenticated ? 1 : 0);
    const char* message = "asd\0";
    quint8 maxChannels = 2;

    quint32 payload = 1 + qstrlen(message)+1 + 1;

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    auto msg = AuthReplyMessage(flag, message, maxChannels);
    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);
    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = AuthReplyMessage::from(&device, payload);

    QCOMPARE(msg.getNewUserName(), QString(message));
    QCOMPARE(msg.getMaxChannels(), maxChannels);

    QCOMPARE(msg.getNewUserName(), otherMsg.getNewUserName());
    QCOMPARE(msg.getErrorMessage(), otherMsg.getErrorMessage());
    QCOMPARE(msg.getMaxChannels(), otherMsg.getMaxChannels());
    QCOMPARE(msg.getMessageType(), otherMsg.getMessageType());
}

void TestMessagesSerialization::configChangeNotifyMessage_data()
{
    QTest::addColumn<quint16>("bpm");
    QTest::addColumn<quint16>("bpi");

    QTest::newRow("120 bpm, 16 bpi") << (quint16)120 << (quint16)16;
    QTest::newRow("80 bpm, 32 bpi") << (quint16)80 << (quint16)32;
    QTest::newRow("140 bpm, 64 bpi") << (quint16)140 << (quint16)64;
    QTest::newRow("140 bpm, 4 bpi") << (quint16)140 << (quint16)4;
}

void TestMessagesSerialization::configChangeNotifyMessage()
{
    //build the message raw data
    QFETCH(quint16, bpm);
    QFETCH(quint16, bpi);

    quint32 payload = 2 + 2;

    QBuffer device;
    device.open(QIODevice::ReadWrite);

    auto msg = ConfigChangeNotifyMessage(bpm, bpi);
    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);
    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = ConfigChangeNotifyMessage::from(&device, payload);

    QCOMPARE(msg.getBpm(), bpm);
    QCOMPARE(msg.getBpi(), bpi);

    QCOMPARE(msg.getBpm(), otherMsg.getBpm());
    QCOMPARE(msg.getBpi(), otherMsg.getBpi());
    QCOMPARE(msg.getMessageType(), otherMsg.getMessageType());
}

void TestMessagesSerialization::userInfoChangeNotifyMessage_data()
{
    QTest::addColumn<quint8>("users");
    QTest::addColumn<quint8>("channelsPerUser");

    //QTest::newRow("No users (empty server)") << (quint8)0 << (quint8)0;
    QTest::newRow("2 users and 1 channel per user") << (quint8)2 << (quint8)1;
    //QTest::newRow("3 users and 2 channel per user") << (quint8)3 << (quint8)2;
}

void TestMessagesSerialization::userInfoChangeNotifyMessage()
{
    QFETCH(quint8, users);
    QFETCH(quint8, channelsPerUser);

    const char* userFullName = "userName";
    const char* channelName = "channelName";
    quint32 payloadPerChannel = 1 + 1 + 2 + 1 + 1 + qstrlen(userFullName) + 2 + qstrlen(channelName) + 2;
    quint32 payload = payloadPerChannel * channelsPerUser * users;

    //build the message raw data
    QBuffer device;
    device.open(QIODevice::ReadWrite);

    UserInfoChangeNotifyMessage msg;
    for (quint8 userID = 0; userID < users; ++userID) {
        for (quint8 channelID = 0; channelID < channelsPerUser; ++channelID) {
            quint8 channelIndex = channelID;
            QString userName = QString("%1%2").arg(userFullName).arg(userID);
            QString chName = QString("%1%2").arg(channelName).arg(channelID);
            UserChannel channel(chName, channelIndex);
            msg.addUserChannel(userName, channel);
        }
    }

    msg.to(&device);

    device.reset();

    auto header = MessageHeader::from(&device);
    QCOMPARE(header.getPayload(), payload);

    auto otherMsg = UserInfoChangeNotifyMessage::from(&device, payload);

    QCOMPARE(otherMsg.getUsers().size(), static_cast<int>(users));

    for (quint8 userIndex = 0; userIndex < users; ++userIndex) {
        User user = otherMsg.getUsers().at(userIndex);
        QVERIFY(userIndex < otherMsg.getUsers().size());
        QString expectedUserName = QString("%1%2").arg(userFullName).arg(userIndex);

        for (quint8 channelIndex = 0; channelIndex < channelsPerUser; ++channelIndex) {
            UserChannel channel = user.getChannels().at(channelIndex);
            QCOMPARE(channel.getIndex(), channelIndex );
            QString expectedChannelName = QString("%1%2").arg(channelName).arg(channelIndex);
            QCOMPARE(channel.getName(), QString(expectedChannelName));
            //QCOMPARE(channel.getUserFullName(), QString(expectedUserName));
        }
    }
}


