#include "JamtabaFactory.h"
#include "loginserver/LoginService.h"

#include <QObject>

Login::LoginService *ReleaseFactory::createLoginService(QObject *parent)
{
    return new Login::LoginService(parent);
}
