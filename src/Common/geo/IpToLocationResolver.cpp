#include "IpToLocationResolver.h"

#include "log/Logging.h"
#include <QRegularExpression>

using geo::Location;
using geo::IpToLocationResolver;
using geo::NullIpToLocationResolver;

Location::Location() :
    countryName("UNKNOWN"),
    countryCode("UNKNOWN"),
    latitude(-200),
    longitude(-200),
    city("UNKNOWN")
{

}

bool Location::isUnknown() const
{
    return countryName == "UNKNOWN" && countryCode == "UNKNOWN";
}

Location::Location(const QString &country, const QString &countryCode, double latitude,
                   double longitude, const QString &city) :
    countryName(sanitize(country)),
    countryCode(sanitize(countryCode)),
    latitude(latitude),
    longitude(longitude),
    city(sanitize(city))
{
    //
}

QString Location::sanitize(const QString &inputString)
{
    static QString htmlTagsPattern("<.+>");
    static QRegularExpression regex(htmlTagsPattern);
    return QString(inputString).replace(regex, QStringLiteral(""));
}

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
