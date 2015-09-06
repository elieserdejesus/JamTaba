#include "IpToLocationLITEResolver.h"

#include <cstring>

//library in http://www.ip2location.com/developers

using namespace Geo;

IpToLocationLITEResolver::IpToLocationLITEResolver(QString binFilePath)
{
    const char* filePath = binFilePath.toStdString().c_str();
    this->IP2LocationObj = IP2Location_open((char*)filePath);
}

Location IpToLocationLITEResolver::resolve(QString ip){

IP2LocationRecord *record = IP2Location_get_all(IP2LocationObj, (char*)ip.toStdString().c_str());

    Location loc( QString(record->country_long), QString(record->country_short) );

/*
printf("%s %s %s %s %s %f %f %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
   record->country_short,
   record->country_long,
   record->region,
   record->city,
   record->isp,
   record->latitude,
   record->longitude,
   record->domain,
   record->zipcode,
   record->timezone,
   record->netspeed,
   record->iddcode,
   record->areacode,
   record->weatherstationcode,
   record->weatherstationname,
   record->mcc,
   record->mnc,
   record->mobilebrand,
   record->elevation,
   record->usagetype);
   */
    IP2Location_free_record(record);
    return loc;
}

IpToLocationLITEResolver::~IpToLocationLITEResolver(){
    IP2Location_close(IP2LocationObj);
}
