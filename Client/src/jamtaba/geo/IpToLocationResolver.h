#ifndef IPTOCOUNTRYRESOLVER_H
#define IPTOCOUNTRYRESOLVER_H

#include <QString>

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
private:
    QString countryName;
    QString countryCode;
    QString city;
    double latitude;
    double longitude;
};

class IpToLocationResolver
{
public:
    virtual Location resolve(QString ip) = 0;
};

}

#endif // IPTOCOUNTRYRESOLVER_H
