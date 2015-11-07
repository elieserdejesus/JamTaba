#include "WebIpToLocationResolver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QTimer>
#include "../log/logging.h"

using namespace Geo;

WebIpToLocationResolver::WebIpToLocationResolver()
    :CACHE_FILE_NAME("cache.bin"){
    QObject::connect(&httpClient, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    //load cache content from file
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if(cacheFile.open(QFile::ReadOnly)){
        QTextStream inputStream(&cacheFile);
        while(!inputStream.atEnd()){
            QString line = inputStream.readLine();
            if(!line.isNull() && !line.isEmpty()){
                QStringList parts = line.split(";");
                if(!parts.isEmpty()){
                    QString ip = parts.at(0);
                    QString countryName = (parts.size() >= 1) ? parts.at(1) : "";
                    QString countryCode = (parts.size() >= 2) ? parts.at(2) : "";
                    locationCache.insert(ip, Geo::Location(countryName, countryCode));
                }
            }
        }
    }
    qCDebug(jtIpToLocation) << "Cache items loaded from file: " << locationCache.size();
}

WebIpToLocationResolver::~WebIpToLocationResolver(){
    qCDebug(jtIpToLocation) << "Saving cache file";
    //save cache content into file
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if(cacheFile.open(QFile::WriteOnly)){
        QTextStream stream(&cacheFile);
        foreach (const QString& ip, locationCache.keys()) {
            Geo::Location location = locationCache[ip];
            stream << ip << ";" << location.getCountryName() << ";" << location.getCountryCode() << endl;
        }
        qCDebug(jtIpToLocation) << locationCache.size() << " items stored in cache file!";
    }
    else{
       qCCritical(jtIpToLocation) << "Can't open the cache file in" << QFileInfo(cacheFile).absoluteFilePath();
    }

}

void WebIpToLocationResolver::replyFinished(QNetworkReply *reply){
    QString ip = reply->property("ip").toString();
    qCDebug(jtIpToLocation) << "request finished for " << ip ;
    if(reply->error() == QNetworkReply::NoError ){
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();
        QString countryName = root["country"].toString();
        QString countryCode = root["country_code"].toString();
        QString city        = "";//root["city"].toString();
        double latitude     = root["latitude"].toDouble();
        double longitude     = root["longitude"].toDouble();
        Location location(countryName, countryCode, city, latitude, longitude);
        locationCache.insert(ip, location);
    }
    else{
        qCDebug(jtIpToLocation) << "error requesting " << ip << ". Returning an empty location!";
    }

    reply->deleteLater();
}

void WebIpToLocationResolver::requestDataFromWebServer(QString ip){
    qCDebug(jtIpToLocation) << "requesting ip " << ip ;
    QNetworkRequest request;
    // http://www.telize.com/geoip/
    // http://geoip.nekudo.com/
    //https://freegeoip.net/json/
    QString serviceUrl = "http://www.telize.com/geoip/";
    request.setUrl(QUrl(serviceUrl + ip));

    QNetworkReply* reply = httpClient.get(request);
    reply->setProperty("ip", QVariant(ip));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
}

void WebIpToLocationResolver::replyError(QNetworkReply::NetworkError e){
    qCCritical(jtIpToLocation) << "Reply error! " << e;
}

Geo::Location WebIpToLocationResolver::resolve(QString ip){
    if(locationCache.contains(ip)){
        qCDebug(jtIpToLocation) << "cache hit for " << ip;
        return locationCache[ip];
    }
    if(!ip.isEmpty() && !ip.isNull()){
        requestDataFromWebServer(ip);
    }
    return Location();//empty location, the next request for same ip probabily return from cache
}

