#ifndef TESTE_SERVICE_H
#define TESTE_SERVICE_H

#include <QObject>
#include "ninjam/ServerMessages.h"
#include "ninjam/UserChannel.h"

using namespace Ninjam;

//these tests are checking if the server messages are extracted correctly from some stream

class TestServerMessages : public QObject
{
    Q_OBJECT

private slots:
    void authChallengeMessage_data();
    void authChallengeMessage();

    void authReplyMessage_data();
    void authReplyMessage();

    void configChangeNotifyMessage_data();
    void configChangeNotifyMessage();

    void userInfoChangeNotifyMessage_data();
    void userInfoChangeNotifyMessage();

    void downloadIntervalBegin();

    void downloadIntervalWrite_data();
    void downloadIntervalWrite();

    void chatMessage_data();
    void chatMessage();

};

#endif
