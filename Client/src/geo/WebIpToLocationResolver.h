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
    virtual Geo::Location resolve(QString ip);
private:
    QMap<QString, Geo::Location> locationCache;
    QNetworkAccessManager httpClient;
    const QString CACHE_FILE_NAME;
    void requestDataFromWebServer(QString ip);
private slots:
    void replyFinished(QNetworkReply*);
    void replyError(QNetworkReply::NetworkError);
};

}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
