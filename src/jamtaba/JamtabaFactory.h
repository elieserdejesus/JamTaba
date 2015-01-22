#pragma once

#include <QObject>

class LoginService;

class JamtabaFactory
{

    virtual LoginService* createLoginService() = 0;
};

//++++++++++++++++++++

class ReleaseFactory : public JamtabaFactory{


    // JamtabaFactory interface
private:
    virtual LoginService *createLoginService(QObject* parent);
};

