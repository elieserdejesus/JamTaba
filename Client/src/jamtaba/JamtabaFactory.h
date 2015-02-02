#pragma once

#include <QObject>

namespace Login {
    class LoginService;
    class LoginServiceListener;
}


class JamtabaFactory
{
public:
    virtual Login::LoginService* createLoginService(Login::LoginServiceListener*) = 0;
    virtual ~JamtabaFactory(){}
};

//++++++++++++++++++++

class ReleaseFactory : public JamtabaFactory{


    // JamtabaFactory interface
private:
    virtual Login::LoginService *createLoginService(Login::LoginServiceListener*);
};

