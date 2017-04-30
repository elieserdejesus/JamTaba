#include <QApplication>
#include "ChatMessagePanel.h"
#include "ChatPanel.h"
#include <QDebug>
#include <QDir>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFile styleSheet(":/Flat.css");
    if (styleSheet.open(QIODevice::ReadOnly))
        app.setStyleSheet(styleSheet.readAll());

    UsersColorsPool colorsPool;
    QStringList botNames("ninjamers.servebeer.com");

    ChatPanel chatPanel(botNames, &colorsPool, nullptr);
    chatPanel.setObjectName(QStringLiteral("ChatPanel"));

    chatPanel.addMessage("UserName", "message", true);
    chatPanel.addMessage("A_big_user_name_", "message", true);
    chatPanel.addMessage("A_big_user_name_", "A big message text to see if the layout is working ok when users decide chat using long texts :)", true);
    chatPanel.addMessage("Jamtaba", "User XXX leave the room", false);
    chatPanel.addMessage("ninjamers.servebeer.com", "Welcome!", true);
    chatPanel.addMessage("Tester", "Helooooooooooooooooooooooooooooooooooooo");
    chatPanel.addMessage("Tester", "Heloooooooooooooooooooooooooooooooooooooooo with other texts to test the behavior");

    chatPanel.show();
    chatPanel.setMinimumHeight(400);
    chatPanel.setMaximumWidth(210); //forcing the mini mode width

    return app.exec();
}
