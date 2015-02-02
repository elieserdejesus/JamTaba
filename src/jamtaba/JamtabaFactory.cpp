#include "JamtabaFactory.h"
#include "network/loginserver/DefaultLoginService.h"

#include <QObject>

Login::LoginService *ReleaseFactory::createLoginService(Login::LoginServiceListener* listener)
{
    return new Login::DefaultLoginService(listener);
}
