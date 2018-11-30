#ifndef JTBA_LOCATION
#define JTBA_LOCATION

#include <QString>
#include <QPointF>

namespace geo
{
    class Location
    {
    public:
        Location(const QString &countryName, const QString &countryCode, double latitude = -200, double longitude = -200, const QString &city = "UNKNOWN");
        Location();

        QPointF getLatLong() const;

        QString getCountryName() const;
        QString getCountryCode() const;

        QString getCity() const;

        bool isUnknown() const;

    private:
        QString countryName;
        QString countryCode;
        QString city;
        double latitude;
        double longitude;

        static QString sanitize(const QString &inputString);
    };

    inline QPointF Location::getLatLong() const
    {
        return QPointF(latitude, longitude);
    }

    inline QString Location::getCountryName() const
    {
        return countryName;
    }

    inline QString Location::getCountryCode() const
    {
        return countryCode;
    }

    inline QString Location::getCity() const
    {
        return city;
    }
}

#endif
