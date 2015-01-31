#pragma once

#include <QList>

class NinjamPublicServersParser {

    virtual QList<NinjaMServer*> getPublicServers() = 0;
    virtual void shutdown(){}
    
};
