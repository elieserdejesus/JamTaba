#ifndef UPNPMANAGER_H
#define UPNPMANAGER_H

#include <QObject>

#include "miniupnpc.h"

class UpNpManager : public QObject
{
    Q_OBJECT

public:
    UpNpManager();
    ~UpNpManager();
    void openPort(quint16 port);
    void closePort(quint16 port);

signals:
    void portOpened(const QString &localIP, const QString &externalIP);
    void portClosed();
    void errorDetected(const QString &msg);

private:
    UPNPDev *devlist;
    UPNPUrls urls;
    IGDdatas data;
};

#endif // UPNPMANAGER_H
