#include <QTest>
#include <QDebug>
#include "TestServerMessages.h"
#include "ninjam/User.h"
#include "ninjam/UserChannel.h"
#include <QDataStream>
#include <QUuid>


void TestServerMessages::chatMessage_data(){
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
    QTest::newRow("Broadcast message") << QString("MSG") << QString("message sender name") <<  QString("message content 😀") << QString("") << QString("");
    QTest::newRow("Topic message") << QString("TOPIC") << QString("message sender name") <<  QString("Topic message content") << QString("") << QString("");
    QTest::newRow("Topic including special symbols") << QString("TOPIC") << QString() <<  QString("😀") << QString() << QString();
    //TOPIC\u0000\u0000Local server \u00E9\u00E3\u00E7\u00F3\u0000\u0000
    QTest::newRow("Joint message") << QString("JOIN") << QString("user name") <<  QString("") << QString("") << QString("");
    QTest::newRow("Part message") << QString("PART") << QString("user name") <<  QString("") << QString("") << QString("");
    QTest::newRow("User count message") << QString("USERCOUNT") << QString("4") <<  QString("8") << QString("") << QString("");
}

void TestServerMessages::chatMessage()
{
    QFETCH(QString, command);
    QFETCH(QString, arg1);
    QFETCH(QString, arg2);
    QFETCH(QString, arg3);
    QFETCH(QString, arg4);

    quint32 payload = command.size() + arg1.toUtf8().size() + arg2.toUtf8().size() + arg3.toUtf8().size() + arg4.toUtf8().size();
    payload += 5; //add a NUL byte for each string

    QStringList args;
    args.append(arg1);
    args.append(arg2);
    args.append(arg3);
    args.append(arg4);

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);

        stream.writeRawData(command.toUtf8().data(), command.size());
        stream << quint8('\0');

        for (const QString &arg : args) {
            QByteArray byteArray = arg.toUtf8();
            stream.writeRawData(byteArray.data(), byteArray.size());
            stream << quint8('\0');
        }
        QCOMPARE(device.size(), (int)payload);
    }

    ServerChatMessage msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getCommand(), ServerChatMessage::commandTypeFromString(command));
    for (int argIndex = 0; argIndex < 4; ++argIndex) {
        QCOMPARE(msg.getArguments().at(argIndex), args.at(argIndex));
    }
}

void TestServerMessages::downloadIntervalWrite_data(){
    QTest::addColumn<quint8>("flags");

    QTest::newRow("download completed") << (quint8)1;
    QTest::newRow("download not completed") << (quint8)0;

}

void TestServerMessages::downloadIntervalWrite(){
    const char* encodedAudioData = "someFakeOggVorbisBytesJustToTest";

    quint32 payload = 16 + 1 + strlen(encodedAudioData);

    QByteArray GUID( QUuid().toRfc4122());
    QCOMPARE(GUID.size(), 16); //we need a 18 bytes GUID

    QFETCH(quint8, flags);

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.writeRawData(GUID, 16);
        stream << flags;
        stream.writeRawData(encodedAudioData, qstrlen(encodedAudioData));
        QCOMPARE(device.size(), (int)payload);
    }

    DownloadIntervalWrite msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getEncodedData(), QByteArray(encodedAudioData));
    QCOMPARE(msg.downloadIsComplete(), flags == 1);
    QCOMPARE(msg.getGUID(), GUID);
}

void TestServerMessages::downloadIntervalBegin()
{
    QByteArray GUID( QUuid().toRfc4122());
    QCOMPARE(GUID.size(), 16); //we need a 18 bytes GUID

    quint32 estimatedSize = 0;
    quint8 fourCC[4];
    quint8 channelIndex = 0;
    const char *userName = "testUserName\0";

    quint32 payload = 16 + 4 + 4 + 1 + qstrlen(userName)+1;

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.writeRawData(GUID, 16);
        stream << estimatedSize;
        for (int k = 0; k < 4; ++k) {
            stream << fourCC[k];
        }
        stream << channelIndex;
        stream.writeRawData( userName, qstrlen(userName)+1);

        QCOMPARE(device.size(), (int)payload);
    }

    DownloadIntervalBegin msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getChannelIndex(), channelIndex);
    QCOMPARE(msg.getEstimatedSize(), estimatedSize);
    QCOMPARE(msg.getGUID(), GUID);
    QCOMPARE(msg.getUserName(), QString(userName));
}

void TestServerMessages::authChallengeMessage_data()
{
    QTest::addColumn<QString>("licenceText");
    QTest::addColumn<quint8>("keepAlivePeriod");

    QTest::newRow("using licence, 30s for keep alive period") << QString("Testing licence\0") << (quint8)30;
    QTest::newRow("using licence, 5s for keep alive period") << QString("Another licence\0") << (quint8)5;
    QTest::newRow("No licence, 15s for keep alive period") << QString("") << (quint8)15;
}

void TestServerMessages::authChallengeMessage()
{
    //build the message raw data
    quint8 challenge[8];
    for (int c = 0; c < 8; ++c) {
        challenge[c] = c;
    }
    QFETCH(quint8, keepAlivePeriod);
    QFETCH(QString, licenceText);

    quint32 serverCapabilities = keepAlivePeriod << 8;//keep alive period value is stored in bytes 8-15
    if(!licenceText.isEmpty())
        serverCapabilities |= 1;//when server has licence the first bit is setted.
    //qInfo() << QString::number(serverCapabilities, 16);


    quint32 protocolVersion = 0x00020000;//fixed value
    quint32 payload = 8 + 4 + 4 + licenceText.size();

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        for (int c = 0; c < 8; ++c)
            stream << challenge[c];
        stream << serverCapabilities;
        stream << protocolVersion;
        stream.writeRawData( licenceText.toStdString().c_str(), licenceText.size());

        QCOMPARE(device.size(), (int)payload);
    }

    ServerAuthChallengeMessage msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getServerKeepAlivePeriod(), serverCapabilities >> 8);
    QCOMPARE(msg.getLicenceAgreement(), QString(licenceText));
    QCOMPARE(msg.getProtocolVersion(), protocolVersion);
    QCOMPARE(msg.getChallenge(), QByteArray((char*)challenge));
    QCOMPARE(msg.serverHasLicenceAgreement(), (bool)(serverCapabilities & 1));
}


void TestServerMessages::authReplyMessage_data()
{
    QTest::addColumn<bool>("userAuthenticated");

    QTest::newRow("User authenticated") << true;
    QTest::newRow("User not authenticated") << false;
}

void TestServerMessages::authReplyMessage()
{
    QFETCH(bool, userAuthenticated);

    //build the message raw data
    quint8 flag = userAuthenticated ? (quint8)1 : (quint8)0;
    const char* message = "asd\0";
    quint8 maxChannels = 2;

    quint32 payload = 1 + qstrlen(message)+1 + 1;

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << flag;
        stream.writeRawData(message, qstrlen(message)+1);
        stream << maxChannels;
        QCOMPARE(device.size(), (int)payload);
    }

    ServerAuthReplyMessage msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getNewUserName(), QString(message));
    QCOMPARE(msg.getMaxChannels(), maxChannels);
}

void TestServerMessages::configChangeNotifyMessage_data()
{
    QTest::addColumn<quint16>("bpm");
    QTest::addColumn<quint16>("bpi");

    QTest::newRow("120 bpm, 16 bpi") << (quint16)120 << (quint16)16;
    QTest::newRow("80 bpm, 32 bpi") << (quint16)80 << (quint16)32;
    QTest::newRow("140 bpm, 64 bpi") << (quint16)140 << (quint16)64;
    QTest::newRow("140 bpm, 4 bpi") << (quint16)140 << (quint16)4;
}

void TestServerMessages::configChangeNotifyMessage()
{
    //build the message raw data
    QFETCH(quint16, bpm);
    QFETCH(quint16, bpi);

    quint32 payload = 2 + 2;

    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream << bpm;
        stream << bpi;
        QCOMPARE(device.size(), (int)payload);
    }

    ServerConfigChangeNotifyMessage msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getBpm(), bpm);
    QCOMPARE(msg.getBpi(), bpi);
}

void TestServerMessages::userInfoChangeNotifyMessage_data()
{
    QTest::addColumn<quint8>("users");
    QTest::addColumn<quint8>("channelsPerUser");

    QTest::newRow("No users (empty server)") << (quint8)0 << (quint8)0;
    QTest::newRow("2 users and 1 channel per user") << (quint8)2 << (quint8)1;
    QTest::newRow("3 users and 2 channel per user") << (quint8)3 << (quint8)2;
}

void TestServerMessages::userInfoChangeNotifyMessage()
{
    QFETCH(quint8, users);
    QFETCH(quint8, channelsPerUser);

    const char* userFullName = "userName";
    const char* channelName = "channelName";
    quint32 payloadPerChannel = 1 + 1 + 2 + 1 + 1 + qstrlen(userFullName)+2 + qstrlen(channelName)+2;
    quint32 payload = payloadPerChannel * channelsPerUser * users;

    //build the message raw data
    QByteArray device;
    {
        QDataStream stream(&device, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        for (quint8 userID = 0; userID < users; ++userID) {
            for (quint8 channelID = 0; channelID < channelsPerUser; ++channelID) {
                quint8 active = 1;
                quint8 channelIndex = channelID;
                quint16 volume = 0;
                quint8 pan = 0;
                quint8 flags = 0;

                stream << active;
                stream << channelIndex;
                stream << volume;
                stream << pan;
                stream << flags;
                stream.writeRawData(userFullName, qstrlen(userFullName));
                stream << (quint8)(userID + 1);
                stream << (quint8)'\0';

                stream.writeRawData(channelName, qstrlen(channelName));
                stream << (quint8)(channelID + 1);
                stream << (quint8)'\0';
            }
        }
    }
    QCOMPARE(device.size(), (int)payload);

    UserInfoChangeNotifyMessage msg(payload);
    QDataStream stream(&device, QIODevice::ReadOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream >> msg; //populate the message

    QCOMPARE(msg.getUsers().size(), (int)users);

    for (quint8 userIndex = 0; userIndex < users; ++userIndex) {
        User user = msg.getUsers().at(userIndex);
        QVERIFY(userIndex < msg.getUsers().size());
        QString expectedUserName = QString(userFullName) + (quint8)(userIndex+1);

        for (quint8 channelIndex = 0; channelIndex < channelsPerUser; ++channelIndex) {
            UserChannel channel = user.getChannels().at(channelIndex);
            QCOMPARE(channel.getIndex(), channelIndex );
            QString expectedChannelName = QString(channelName) + (quint8)(channelIndex+1);
            QCOMPARE(channel.getName(), QString(expectedChannelName));
            QCOMPARE(channel.getUserFullName(), QString(expectedUserName));
        }
    }
}


