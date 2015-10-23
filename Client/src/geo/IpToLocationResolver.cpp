#include "IpToLocationResolver.h"

#include <QDebug>

using namespace Geo ;

Location::Location()
    :countryName("UNKNOWN"), countryCode("UNKNOWN"),
      city("UNKNOWN"),
      latitude(-200), longitude(-200){

}

bool Location::isUnknown() const{
    return countryName == "UNKNOWN" && countryCode == "UNKNOWN";
}

Location::Location(QString country, QString countryCode)
    : countryName(country), countryCode(countryCode),
      city("UNKNOWN"),
      latitude(-200), longitude(-200){

}

Location::Location(QString country, QString countryCode, QString city, double latitude, double longitude)
    : countryName(country), countryCode(countryCode),
      city(city),
      latitude(latitude), longitude(longitude){

}
//+++++++++++++++++++++++++++++++++++++++++++++++++++
IpToLocationResolver::~IpToLocationResolver(){
    qDebug() << "IpToLocationResolver destructor";
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++
Location NullIpToLocationResolver::resolve(QString ip){
    Q_UNUSED(ip)
    return Geo::Location();
}


