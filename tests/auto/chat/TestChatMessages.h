#ifndef TEST_ADMIN_COMMANDS_H
#define TEST_ADMIN_COMMANDS_H

#include <QObject>

class TestAdminCommands : public QObject
{
    Q_OBJECT

private slots:

    void validAdminCommands_data();
    void validAdminCommands();

    void invalidAdminCommands_data();
    void invalidAdminCommands();

    void validPrivateMessage();
    void validPrivateMessage_data();

    void invalidPrivateMessage();
    void invalidPrivateMessage_data();

};

#endif // TEST_ADMIN_COMMANDS_H
