#include "FreeGeoIpToLocationResolver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkAccessManager>


Q_LOGGING_CATEGORY(freeGeoIp, "freeGeoIp")

using namespace Geo;

FreeGeoIpToLocationResolver::FreeGeoIpToLocationResolver()
{

}

Location FreeGeoIpToLocationResolver::resolve(QString ip){
    if(locationCache.contains(ip)){
        qCDebug(freeGeoIp) << "cache hit for " << ip;
        return locationCache[ip];
    }

    qCDebug(freeGeoIp) << "requesting ip " << ip ;
    QNetworkRequest request;
    request.setUrl(QUrl("https://freegeoip.net/json/" + ip));
    QNetworkAccessManager httpClient;
    QNetworkReply* reply = httpClient.get(request);

    QEventLoop loop;
    QObject::connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    loop.exec();
    qCDebug(freeGeoIp) << "request finished for " << ip ;
    if(reply->error() == QNetworkReply::NoError){
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();
        QString countryName = root["country_name"].toString();
        QString countryCode = root["country_code"].toString();
        QString city        = root["city"].toString();
        double latitude     = root["latitude"].toDouble();
        double longitude     = root["longitude"].toDouble();
        Location location(countryName, countryCode, city, latitude, longitude);
        locationCache.insert(ip, location);
        return location;
    }
    else{
        qCDebug(freeGeoIp) << "error requesting " << ip << ". Returning an empty location!";
    }

    reply->deleteLater();
    return Location();//empty location
}

