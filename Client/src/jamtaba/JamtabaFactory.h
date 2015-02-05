#pragma once

#include <QObject>

namespace Login {
    class LoginService;
}


class JamtabaFactory
{
public:
    virtual Login::LoginService* createLoginService(QObject* parent=0) = 0;
    virtual ~JamtabaFactory(){}
};

//++++++++++++++++++++

class ReleaseFactory : public JamtabaFactory{


    // JamtabaFactory interface
private:
    virtual Login::LoginService *createLoginService(QObject* parent=0);
};

