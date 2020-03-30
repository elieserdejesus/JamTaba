#ifndef IPTOLOCATIONLITERESOLVER_H
#define IPTOLOCATIONLITERESOLVER_H

#include "IpToLocationResolver.h"

#include "IP2Location.h"


//class IP2Location;

namespace Geo{

class IpToLocationLITEResolver : public IpToLocationResolver
{
public:
    explicit IpToLocationLITEResolver(QString binFilePath);
    ~IpToLocationLITEResolver();
    Location resolve(QString ip);
private:
IP2Location* IP2LocationObj;

};

}
#endif // IPTOLOCATIONLITERESOLVER_H
