#include "JamtabaFactory.h"
#include "network/loginserver/DefaultLoginService.h"

#include <QObject>

LoginService *ReleaseFactory::createLoginService(QObject* parent)
{
    return new DefaultLoginService(parent);
}
