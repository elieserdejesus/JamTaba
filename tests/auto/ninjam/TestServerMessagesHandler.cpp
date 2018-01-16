#include "TestServerMessagesHandler.h"
#include "ninjam/ServerMessagesHandler.h"
#include "ninjam/ServerMessages.h"
#include <QFile>
#include <QFileInfo>
#include <QDir>

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
        currentHeader.reset(extractNextMessageHeader());
        ServerAuthChallengeMessage authChallenge(currentHeader->payload);
        stream >> authChallenge;
        QVERIFY(authChallenge.serverHasLicenceAgreement());//just a simple check

        currentHeader.reset(extractNextMessageHeader());
        ServerAuthReplyMessage replyMessage(currentHeader->payload);
        stream >> replyMessage;
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
        currentHeader.reset(extractNextMessageHeader());
        ServerAuthChallengeMessage authChallenge(currentHeader->payload);
        stream >> authChallenge;
        QVERIFY(authChallenge.serverHasLicenceAgreement());//just a simple check

        //auth reply
        currentHeader.reset(extractNextMessageHeader());
        ServerAuthReplyMessage replyMessage(currentHeader->payload);
        stream >> replyMessage;
        QVERIFY(replyMessage.getNewUserName().startsWith("wiresharker"));
        QVERIFY(replyMessage.getMaxChannels() == 2);
        QVERIFY(replyMessage.userIsAuthenticated());

        //serverConfigChangeNotify
        currentHeader.reset(extractNextMessageHeader());
        ServerConfigChangeNotifyMessage serverConfig(currentHeader->payload);
        stream >> serverConfig;
        QVERIFY(serverConfig.getBpi() == 16);
        QVERIFY(serverConfig.getBpm() == 125);

        //userInfoChangeNotify
        currentHeader.reset(extractNextMessageHeader());
        UserInfoChangeNotifyMessage userInfo(currentHeader->payload);
        stream >> userInfo;

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
        currentHeader.reset(extractNextMessageHeader());
        ServerChatMessage topicMessage(currentHeader->payload);
        stream >> topicMessage;

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


