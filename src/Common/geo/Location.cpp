#include "Location.h"

#include <QRegularExpression>

using namespace geo;

Location::Location() :
    countryName("UNKNOWN"),
    countryCode("UNKNOWN"),
    city("UNKNOWN"),
    latitude(-200),
    longitude(-200)
{
    //
}

bool Location::isUnknown() const
{
    return countryName == "UNKNOWN" && countryCode == "UNKNOWN";
}

Location::Location(const QString &country, const QString &countryCode, double latitude, double longitude, const QString &city) :
    countryName(sanitize(country)),
    countryCode(sanitize(countryCode)),
    city(sanitize(city)),
    latitude(latitude),
    longitude(longitude)
{
    //
}

QString Location::sanitize(const QString &inputString)
{
    static QString htmlTagsPattern("<.+>");
    static QRegularExpression regex(htmlTagsPattern);
    return QString(inputString).replace(regex, QStringLiteral(""));
}
