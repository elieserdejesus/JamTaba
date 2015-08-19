#ifndef MAXMINDIPTOLOCATIONRESOLVER_H
#define MAXMINDIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <maxminddb.h>

namespace Geo {



class MaxMindIpToLocationResolver : public IpToLocationResolver
{
public:
    explicit MaxMindIpToLocationResolver(QString databasePath);
    ~MaxMindIpToLocationResolver();
    virtual Location resolve(QString ip);
private:
    MMDB_s mmdb_s;
};

}

#endif // MAXMINDIPTOLOCATIONRESOLVER_H
