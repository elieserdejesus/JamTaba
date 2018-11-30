#include "WebIpToLocationResolver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QPointF>
#include <QSettings>
#include <QApplication>

#include "log/Logging.h"
#include "Location.h"

using geo::WebIpToLocationResolver;

// Alternative servers private implementation strategies
const int MaxServersAlternatives = 2;

WebIpToLocationResolver::WebIpToLocationResolver(const QDir &cacheDir) :
    cache(cacheDir)
{
    connect(&httpClient, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
}

WebIpToLocationResolver::~WebIpToLocationResolver()
{
    //
}

void WebIpToLocationResolver::replyFinished(QNetworkReply *reply)
{
    QString ip = reply->property("ip").toString();
    QString language = reply->property("language").toString();
    int retryCount = reply->property("retryCount").toInt();

    if (language != cache.getCurrentLanguage())
        return; // discard the received data if the language was changed since the last request.

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isEmpty() || !doc.isObject()) {
            qCritical() << "Json document is empty or is not an json object!";
            return;
        }

        QJsonObject root = doc.object();
        if (!root.contains("country_name")) {
            if (root.contains("error")) {
                auto error = root["error"].toObject();
                auto code = error.contains("code") ? error["code"].toInt() : -1;
                if (retryCount < MaxServersAlternatives) {
                    qDebug() << "Error " << code << " received, trying alternative server ...";
                    requestDataFromWebService(ip, retryCount + 1);
                } else {
                    qCritical() << "All servers failed, no more alternatives available";
                }
                return;
            } else {
                qCritical() << "The root json object not contains 'country_name'";
                return;
            }
        }

        if (!root.contains("country_code")) {
            qCritical() << "The root json object not contains 'country_name'";
            return;
        }

        auto countryName = root["country_name"].toString();
        auto countryCode = root["country_code"].toString();
        auto latitude = -200.0;
        auto longitude = -200.0;

        if (root.contains("latitude") && root.contains("longitude")) {
            latitude = root["latitude"].toDouble();
            longitude = root["longitude"].toDouble();
            qCDebug(jtIpToLocation) << "Data received IP:" << ip << " Lang:" << language << " country code:" << countryCode
                                    << " country name:" << countryName << "lat:" << latitude << " long:" << longitude;
        } else {
            qCritical() << "The json 'location' object not contains 'latidude' or 'longitude' entries";
        }

        Location location(countryName, countryCode, latitude, longitude);
        cache.add(ip, location);
        emit ipResolved(ip, location);
    } else {
        qCDebug(jtIpToLocation) << "error requesting " << ip << ". Returning an empty location!";
    }

    reply->deleteLater();
}

// At moment the current api is http://api.ipapi.com/ (the replacement for Nekudo api). Another option is https://freegeoip.net/json/
void WebIpToLocationResolver::requestDataFromWebService(const QString &ip, int retryCount)
{
    qCDebug(jtIpToLocation) << "requesting ip " << ip;

    QNetworkRequest request;

    static QString ipApiKey;
    static QString ipStackKey;

    if (ipApiKey.isEmpty()) {
        QSettings settings(QCoreApplication::applicationName());
        ipApiKey = settings.value("ipApiKey", "22f433857895c84347e949db234af11f").toString();
        ipStackKey = settings.value("ipStackKey", "7dfb5855025fa60f5ce082a4c376091a").toString();
    }

    QString serviceUrl = "http://api.ipapi.com";
    QString accessKey(ipApiKey);

    QString alternativeServiceUrl1 = "http://api.ipstack.com/";
    QString alternativeAccessKey1(ipStackKey);

    QString lang = sanitizeLanguageCode(cache.getCurrentLanguage());
    if (!canTranslateCountryName(lang))
        lang = "en";

    // URL FORMAT: i.e. http://api.ipapi.com/{ip}?access_key={key}&language={lang}
    switch (retryCount) {
    case 0:
        request.setUrl(QUrl(QString("%1/%2?access_key=%3&language=%4")
                            .arg(serviceUrl, ip, accessKey, lang)));
        break;
    default:
        request.setUrl(QUrl(QString("%1/%2?access_key=%3&language=%4")
                            .arg(alternativeServiceUrl1, ip, alternativeAccessKey1, lang)));
        break;
    }

    QNetworkReply *reply = httpClient.get(request);
    reply->setProperty("ip", QVariant(ip));
    reply->setProperty("language", QVariant(cache.getCurrentLanguage()));
    reply->setProperty("retryCount", retryCount);

    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,
                     SLOT(replyError(QNetworkReply::NetworkError)));
}

bool WebIpToLocationResolver::canTranslateCountryName(const QString &currentLanguage)
{
    // nekudo.com api is supporting only some languages
    static QStringList supportedCountryTranslationLanguages("en");
    supportedCountryTranslationLanguages << "de" << "es" << "fr" << "ja" << "pt" << "ru" << "zh";

    return supportedCountryTranslationLanguages.contains(sanitizeLanguageCode(currentLanguage));
}

void WebIpToLocationResolver::replyError(QNetworkReply::NetworkError e)
{
    qCritical() << "Reply error! " << e;
}

QString WebIpToLocationResolver::sanitizeLanguageCode(const QString &languageCode)
{
    if (languageCode.isEmpty())
        return "en";

    if (languageCode.size() > 2)
        return languageCode.toLower().left(2);

    return languageCode;
}

geo::Location WebIpToLocationResolver::resolve(const QString &ip, const QString &languageCode)
{
    // check for language changes
    QString code = sanitizeLanguageCode(languageCode);
    if (code != cache.getCurrentLanguage())
        cache.setCurrentLanguage(code);

    auto location = cache.getLocation(ip);

    if (!ip.isEmpty() && location.isUnknown()) // request from API if location is unknown (not cached yet)
        requestDataFromWebService(ip, 0);

    return location;// unknown or cached location
}
