#include "IpToLocationResolver.h"

#include "log/Logging.h"
#include <QRegularExpression>

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
    countryName(sanitize(country)),
    countryCode(sanitize(countryCode)),
    city(sanitize(city)),
    latitude(latitude),
    longitude(longitude)
{
}

QString Location::sanitize(const QString &inputString)
{
    static QString htmlTagsPattern("<.+>");
    static QRegularExpression regex(htmlTagsPattern);
    return QString(inputString).replace(regex, QStringLiteral(""));
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++
IpToLocationResolver::~IpToLocationResolver()
{
    qCDebug(jtIpToLocation) << "IpToLocationResolver destructor";
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++
Location NullIpToLocationResolver::resolve(const QString &ip, const QString &languageCode)
{
    Q_UNUSED(ip)
    Q_UNUSED(languageCode)
    return Geo::Location();
}
