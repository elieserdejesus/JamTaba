#include "JamtabaFactory.h"
#include "network/loginserver/DefaultLoginService.h"

#include <QObject>

LoginService *ReleaseFactory::createLoginService(LoginServiceListener* listener)
{
    return new DefaultLoginService(listener);
}
