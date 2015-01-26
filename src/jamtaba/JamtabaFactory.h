#pragma once

#include <QObject>

class LoginService;
class LoginServiceListener;

class JamtabaFactory
{
public:
    virtual LoginService* createLoginService(LoginServiceListener*) = 0;
    virtual ~JamtabaFactory(){}
};

//++++++++++++++++++++

class ReleaseFactory : public JamtabaFactory{


    // JamtabaFactory interface
private:
    virtual LoginService *createLoginService(LoginServiceListener*);
};

