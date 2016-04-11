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
    Geo::Location resolve(const QString &ip, const QString &languageCode) override;
private:
    QMap<QString, Geo::Location> locationCache;
    QNetworkAccessManager httpClient;

    void requestDataFromWebServer(const QString &ip);

    static const QString COUNTRY_CODES_FILE;
    static const QString COUNTRY_NAMES_FILE_PREFIX;
private slots:
    void replyFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);
};
}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
