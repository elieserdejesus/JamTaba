#ifndef IPTOCOUNTRYRESOLVER_H
#define IPTOCOUNTRYRESOLVER_H

#include <QString>
#include <QObject>

namespace geo
{
    class Location;

    class IpToLocationResolver : public QObject
    {
        Q_OBJECT

    public:
        virtual Location resolve(const QString &ip, const QString &languageCode) = 0;
        virtual ~IpToLocationResolver();

    signals:
        void ipResolved(const QString &ip, const geo::Location &location);
    };

    class NullIpToLocationResolver : public IpToLocationResolver
    {
    public:
        Location resolve(const QString &ip, const QString &languageCode) override;
    };
}

#endif // IPTOCOUNTRYRESOLVER_H
