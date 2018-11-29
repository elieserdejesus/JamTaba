#ifndef IPTOCOUNTRYRESOLVER_H
#define IPTOCOUNTRYRESOLVER_H

#include <QString>
#include <QObject>

namespace geo
{
    class Location
    {
    public:
        Location(const QString &countryName, const QString &countryCode, double latitude = -200, double longitude = -200, const QString &city = "UNKNOWN");
        Location();

        double getLatitude() const;
        double getLongitude() const;

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

    inline double Location::getLatitude() const
    {
        return latitude;
    }

    inline double Location::getLongitude() const
    {
        return longitude;
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

    class IpToLocationResolver : public QObject
    {
        Q_OBJECT

    public:
        virtual Location resolve(const QString &ip, const QString &languageCode) = 0;
        virtual ~IpToLocationResolver();

    signals:
        void ipResolved(const QString &ip);
    };

    class NullIpToLocationResolver : public IpToLocationResolver
    {
    public:
        Location resolve(const QString &ip, const QString &languageCode) override;
    };
}

#endif // IPTOCOUNTRYRESOLVER_H
