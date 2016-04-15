#ifndef IPTOCOUNTRYRESOLVER_H
#define IPTOCOUNTRYRESOLVER_H

#include <QString>
#include <QObject>

namespace Geo {
class Location
{
public:
    Location(const QString &countryName, const QString &countryCode, const QString &city = "UNKNOWN", double latitude = -200,
             double longitude = -200);
    Location();

    inline double getLatitude() const
    {
        return latitude;
    }

    inline double getLongitude() const
    {
        return longitude;
    }

    inline QString getCountryName() const
    {
        return countryName;
    }

    inline QString getCountryCode() const
    {
        return countryCode;
    }

    inline QString getCity()    const
    {
        return city;
    }

    bool isUnknown() const;
private:
    QString countryName;
    QString countryCode;
    QString city;
    double latitude;
    double longitude;

    static QString sanitize(const QString &inputString);
};

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
