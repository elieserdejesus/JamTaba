#ifndef TESTSERVERMESSAGESHANDLER_H
#define TESTSERVERMESSAGESHANDLER_H

#include <QObject>
#include <QTest>

class TestServerMessagesHandler : public QObject
{
    Q_OBJECT

private slots:
    void handShakeMessages();//test if ninjam server handshake messages are received and handled in the correct order
    void connectInFullServer();//connect in a full server
};

#endif // TESTSERVERMESSAGESHANDLER_H
