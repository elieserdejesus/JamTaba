#ifndef TESTE_SERVER_H
#define TESTE_SERVER_H

#include <QObject>

class TestServerInfo : public QObject
{
    Q_OBJECT

private slots:
    void addUser();

    void addUserChannel_data();
    void addUserChannel();

    void removeUserChannel_data();
    void removeUserChannel();

    void updateUserChannel_data();
    void updateUserChannel();
};

#endif
