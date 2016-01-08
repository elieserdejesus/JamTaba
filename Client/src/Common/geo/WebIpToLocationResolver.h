#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <QMap>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Geo {
class WebIpToLocationResolver : public IpToLocationResolver
{
    Q_OBJECT
public:
    WebIpToLocationResolver();
    ~WebIpToLocationResolver();
    Geo::Location resolve(const QString &ip) override;
private:
    QMap<QString, Geo::Location> locationCache;
    QNetworkAccessManager httpClient;
    const QString CACHE_FILE_NAME;
    void requestDataFromWebServer(const QString &ip);
private slots:
    void replyFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);
};
}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
