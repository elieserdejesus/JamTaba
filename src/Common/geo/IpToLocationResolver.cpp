#include "IpToLocationResolver.h"

#include "log/Logging.h"
#include "geo/Location.h"

using geo::Location;
using geo::IpToLocationResolver;
using geo::NullIpToLocationResolver;

IpToLocationResolver::~IpToLocationResolver()
{
    qCDebug(jtIpToLocation) << "IpToLocationResolver destructor";
}

Location NullIpToLocationResolver::resolve(const QString &ip, const QString &languageCode)
{
    Q_UNUSED(ip)
    Q_UNUSED(languageCode)
    return geo::Location();
}
