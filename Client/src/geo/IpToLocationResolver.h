#ifndef IPTOCOUNTRYRESOLVER_H
#define IPTOCOUNTRYRESOLVER_H

#include <QString>
#include <QObject>

namespace Geo {

class Location{
public:
    Location(QString countryName, QString countryCode, QString city, double latitude, double longitude);
    explicit Location(QString countryName, QString countryCode);
    Location();

    inline double getLatitude() const{ return latitude; }
    inline double getLongitude()const{ return longitude;}
    inline QString getCountryName() const{ return countryName;  }
    inline QString getCountryCode() const {return countryCode; }
    inline QString getCity()    const{ return city;     }
    bool isUnknown() const;
private:
    QString countryName;
    QString countryCode;
    QString city;
    double latitude;
    double longitude;
};

class IpToLocationResolver : public QObject
{

public:
    virtual Location resolve(QString ip) = 0;
    virtual ~IpToLocationResolver();

};

class NullIpToLocationResolver : public IpToLocationResolver{
public:
    Location resolve(QString ip);
};

}

#endif // IPTOCOUNTRYRESOLVER_H
