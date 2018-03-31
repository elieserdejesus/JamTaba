#include <QApplication>
#include "ChatMessagePanel.h"
#include "UsersColorsPool.h"
#include "EmojiManager.h"
#include "ChatPanel.h"
#include <QDebug>
#include <QDir>
#include <QtTest/QTest>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    UsersColorsPool colorsPool;
    QStringList botNames("ninjamers.servebeer.com");

    EmojiManager emojiManager("", "");

    ChatPanel chatPanel(botNames, &colorsPool, nullptr, &emojiManager);
    chatPanel.setTopicMessage("Server topic message");
    chatPanel.setObjectName(QStringLiteral("ChatPanel"));
    chatPanel.setConnectedUsers(QStringList() << "Tester" << "Tester 2" << "Tester 3");

    QString localUserName("Tester");
    chatPanel.addMessage(localUserName, "UserName", "message", true);
    chatPanel.addMessage(localUserName, "A_big_user_name_", "message", true);
    chatPanel.addMessage(localUserName, "A_big_user_name_", "A big message text to see if the layout is working ok when users decide chat using long texts :)", true);
    chatPanel.addMessage(localUserName, "Jamtaba", "User XXX leave the room", false);
    chatPanel.addMessage(localUserName, "ninjamers.servebeer.com", "Welcome!", true);
    chatPanel.addMessage(localUserName, "Tester", "Helooooooooooooooooooooooooooooooooooooo");
    chatPanel.addMessage(localUserName, "Tester", "Heloooooooooooooooooooooooooooooooooooooooo with other texts to test the behavior");

    chatPanel.show();
    chatPanel.setMinimumHeight(400);
    chatPanel.setMaximumWidth(250); //forcing the mini mode width

    return app.exec();
}
