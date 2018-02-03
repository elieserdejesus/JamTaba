#include "TestServerMessagesHandler.h"
#include "ninjam/client/ServerMessagesHandler.h"
#include "ninjam/client/ServerMessages.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>

using namespace ninjam::client;
using namespace ninjam;

/**
Simulate the connection in a full server using data collected with Wireshark. The sequence of received message are a
ServerAuthChallenge followed by a ServerAuthReply containing a error message "server full".
*/

class FullServerMessagesHandler : public ServerMessagesHandler
{
public:
    FullServerMessagesHandler():
        ServerMessagesHandler(nullptr){

    }

    void handleAllMessages() override
    {
        currentHeader = MessageHeader::from(device);
        auto authChallenge = AuthChallengeMessage::from(device, currentHeader.getPayload());

        QVERIFY(authChallenge.serverHasLicenceAgreement());//just a simple check

        currentHeader = MessageHeader::from(device);
        auto replyMessage = AuthReplyMessage::from(device, currentHeader.getPayload());

        QCOMPARE(replyMessage.getErrorMessage(), QString("server full"));
    }
};

void TestServerMessagesHandler::connectInFullServer()
{
    FullServerMessagesHandler messagesHandler;
    QFile wiresharkFile(":/wireshark data/full server.data");
    //check if the wireshark data file can be opened
    QVERIFY2( wiresharkFile.open(QIODevice::ReadOnly), wiresharkFile.errorString().toStdString().c_str());

    messagesHandler.initialize(&wiresharkFile);
    messagesHandler.handleAllMessages();
}


//-----------------------------------------------------------------------------------------------------------------

/**
this test is reading some data extracted from a real ninjam server communication (ninbot.com) using Wireshark.
This test check if the first messages (handshake) are readed in the correct order.
*/

class HandshakeMessagesHandler : public ServerMessagesHandler
{
public:
    HandshakeMessagesHandler():
        ServerMessagesHandler(nullptr){

    }

    void handleAllMessages() override
    {
        //auth challenge
        currentHeader = MessageHeader::from(device);
        auto authChallenge = AuthChallengeMessage::from(device, currentHeader.getPayload());

        QVERIFY(authChallenge.serverHasLicenceAgreement());//just a simple check

        //auth reply
        currentHeader = MessageHeader::from(device);
        auto replyMessage = AuthReplyMessage::from(device,currentHeader.getPayload());

        QVERIFY(replyMessage.getNewUserName().startsWith("wiresharker"));
        QVERIFY(replyMessage.getMaxChannels() == 2);
        QVERIFY(replyMessage.userIsAuthenticated());

        //serverConfigChangeNotify
        currentHeader = MessageHeader::from(device);
        auto serverConfig = ConfigChangeNotifyMessage::from(device, currentHeader.getPayload());

        QVERIFY(serverConfig.getBpi() == 16);
        QVERIFY(serverConfig.getBpm() == 125);

        //userInfoChangeNotify
        currentHeader = MessageHeader::from(device);
        auto userInfo = UserInfoChangeNotifyMessage::from(device, currentHeader.getPayload());

        //check if all expected users and the user channels are in the list
        QMap<QString, QStringList> expectedUsersChannels;
        QStringList ninbotChannels("recording 10:25");
        ninbotChannels.append("channel0");
        expectedUsersChannels.insert("ninbot", ninbotChannels);
        expectedUsersChannels.insert("PowaCord@98.215.146.x", QStringList("toothcup"));
        expectedUsersChannels.insert("Torben_Scharling@185.10.223.x", QStringList("new channel"));
        expectedUsersChannels.insert("meilo@91.39.197.x", QStringList("default channel"));

        QList<User> connectedUsers = userInfo.getUsers();
        QVERIFY(connectedUsers.size() == expectedUsersChannels.size());
        foreach (const User &user, connectedUsers) {
            QVERIFY(expectedUsersChannels.contains(user.getFullName()));
            QStringList expectedChannels = expectedUsersChannels[user.getFullName()];
            QVERIFY(!expectedChannels.isEmpty());
            foreach (const UserChannel &channel, user.getChannels()) {
                QVERIFY( expectedChannels.contains(channel.getName()));
            }
        }


        //check the topic message
        currentHeader = MessageHeader::from(device);
        auto topicMessage = ServerChatMessage::from(device, currentHeader.getPayload());

        QVERIFY(topicMessage.getCommand() == ChatCommandType::TOPIC);
        QCOMPARE(topicMessage.getArguments().at(1), QString("\"Happy New Year 2016 ALL!!\""));
    }
};

void TestServerMessagesHandler::handShakeMessages()
{
    HandshakeMessagesHandler messagesHandler;
    QFile wiresharkFile(":/wireshark data/ninbot 4 players connected.data");
    //check if the wireshark data file can be opened
    QVERIFY2( wiresharkFile.open(QIODevice::ReadOnly), wiresharkFile.errorString().toStdString().c_str());

    messagesHandler.initialize(&wiresharkFile);
    messagesHandler.handleAllMessages();

}


