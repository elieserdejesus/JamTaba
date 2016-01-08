#include "IpToLocationResolver.h"

#include "log/Logging.h"

using namespace Geo;

Location::Location() :
    countryName("UNKNOWN"),
    countryCode("UNKNOWN"),
    city("UNKNOWN"),
    latitude(-200),
    longitude(-200)
{
}

bool Location::isUnknown() const
{
    return countryName == "UNKNOWN" && countryCode == "UNKNOWN";
}

Location::Location(const QString &country, const QString &countryCode, const QString &city, double latitude,
                   double longitude) :
    countryName(country),
    countryCode(countryCode),
    city(city),
    latitude(latitude),
    longitude(longitude)
{
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++
IpToLocationResolver::~IpToLocationResolver()
{
    qCDebug(jtIpToLocation) << "IpToLocationResolver destructor";
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++
Location NullIpToLocationResolver::resolve(const QString &ip)
{
    Q_UNUSED(ip)
    return Geo::Location();
}
