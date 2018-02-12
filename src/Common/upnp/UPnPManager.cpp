#include "UpNpManager.h"

#include <QDebug>

#ifdef _WIN32
#include <winsock2.h>
#define snprintf _snprintf
#else
/* for IPPROTO_TCP / IPPROTO_UDP */
#include <netinet/in.h>
#endif

#include "miniwget.h"
#include "upnpcommands.h"
#include "upnperrors.h"
#include "miniupnpcstrings.h"

UPnPManager::UPnPManager() :
    QObject(nullptr),
    devlist(nullptr)
{

#ifdef _WIN32
    WSADATA wsaData;
    int nResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if(nResult != NO_ERROR)
    {
        qCritical() << "WSAStartup() failed";
    }
#endif

}

UPnPManager::~UPnPManager()
{
#ifdef _WIN32
    int nResult = WSACleanup();
    if(nResult != NO_ERROR) {
        fprintf(stderr, "WSACleanup() failed.\n");
    }
#endif /* _WIN32 */

    if (devlist)
        freeUPNPDevlist(devlist);
}

void UPnPManager::openPort(quint16 port)
{
    int ipv6 = 0;
    unsigned char ttl = 2;	/* defaulting to 2 */
    int localport = UPNP_LOCAL_PORT_ANY;
    int error = 0;

    devlist = upnpDiscover(2000, 0, 0, localport, ipv6, ttl, &error);

    if(!devlist) {
        emit(QString("Error discovering UpNp devices (code: %1)").arg(error));
        return;
    }


    char lanaddr[64] = "unset";	/* my ip address on the LAN */

    int result = UPNP_GetValidIGD(devlist, &urls, &data, lanaddr, sizeof(lanaddr));

    if (result == 2 || result == 3) {
        if (result == 2)
            emit errorDetected(QString("Found a (not connected?) IGD : %1")
                       .arg(QString(urls.controlURL)));
        else
            emit errorDetected(QString("UPnP device found. Is it an IGD ? : %1").arg(QString(urls.controlURL)));

        return;
    }

    char externalIPAddress[40];
    result = UPNP_GetExternalIPAddress(urls.controlURL, data.first.servicetype, externalIPAddress);

    if(result != UPNPCOMMAND_SUCCESS) {
        emit errorDetected("GetExternalIPAddress failed");
        return;
    }

    QByteArray ba = QString::number(port).toLatin1();
    const char *eport = ba.data();
    const char *iport = eport;
    const char *iaddr = lanaddr;
    const char * description = "UpNpManager";
    const char *proto = "TCP";
    const char *leaseDuration = "0";

    result = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, eport, iport, iaddr, description, proto, 0, leaseDuration);
    if(result != UPNPCOMMAND_SUCCESS) {
        emit errorDetected(QString("AddPortMapping(%1, %2, %3) failed with code %4 (%5)")
                   .arg(eport)
                   .arg(iport)
                   .arg(iaddr)
                   .arg(result)
                   .arg(strupnperror(result)));
        return;
    }

    emit portOpened(QString(iaddr), QString(externalIPAddress));
}

void UPnPManager::closePort(quint16 port)
{
    const char *eport = QString::number(port).toStdString().c_str();
    int result = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype, eport, "TCP", nullptr);
    if (result == 0)
        emit portClosed();
    else
        emit errorDetected(QString(strupnperror(result)));
}
