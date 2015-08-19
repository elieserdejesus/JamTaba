#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <QMap>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(freeGeoIp)

namespace Geo {

class FreeGeoIpToLocationResolver : public IpToLocationResolver
{
public:
    FreeGeoIpToLocationResolver();
    virtual Location resolve(QString ip);
private:
    QMap<QString, Geo::Location> locationCache;
};

}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
