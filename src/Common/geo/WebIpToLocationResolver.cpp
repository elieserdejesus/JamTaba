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
#include <QDataStream>
#include <QTimer>
#include "log/Logging.h"

using namespace Geo;

const QString WebIpToLocationResolver::COUNTRY_CODES_FILE = "country_codes_cache.bin";
const QString WebIpToLocationResolver::COUNTRY_NAMES_FILE_PREFIX = "country_names_cache"; //the language code will be concatenated

WebIpToLocationResolver::WebIpToLocationResolver()
    :currentLanguage("en") //using english as default language
{
    QObject::connect(&httpClient, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    loadCountryCodesFromFile();
    loadCountryNamesFromFile(currentLanguage); //loading the english country names by default
}

WebIpToLocationResolver::~WebIpToLocationResolver()
{
    saveCountryCodesToFile();
    saveCountryNamesToFile();
}

void WebIpToLocationResolver::saveCountryNamesToFile()
{
    QString filename = buildFileNameFromLanguage(currentLanguage);
    if (saveMapToFile(filename, countryNamesCache))
        qCDebug(jtIpToLocation) << countryNamesCache.size() << " country names stored in " << filename;
    else
        qCritical() << "Can't save country names in the file " << filename;
}

void WebIpToLocationResolver::saveCountryCodesToFile()
{
    if (saveMapToFile(COUNTRY_CODES_FILE, countryCodesCache))
        qCDebug(jtIpToLocation) << countryCodesCache.size() << " country codes stored in " << COUNTRY_CODES_FILE;
    else
        qCritical() << "Can't save country codes in the file " << COUNTRY_CODES_FILE;
}

bool WebIpToLocationResolver::saveMapToFile(const QString &fileName, const QMap<QString, QString> &map)
{
    if (map.isEmpty())
        return true;

    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if(cacheFile.open(QFile::WriteOnly)){
        QDataStream stream(&cacheFile);
        stream << map;
        return true;
    }
    return false;
}

void WebIpToLocationResolver::replyFinished(QNetworkReply *reply){
    QString ip = reply->property("ip").toString();
    QString language = reply->property("language").toString();

    if (language != currentLanguage)
        return; //discard the received data if the language was changed since the last request.

    if(reply->error() == QNetworkReply::NoError ){
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();
        QJsonObject countryObject = root["country"].toObject();
        QString countryName = countryObject["name"].toString();
        QString countryCode = countryObject["code"].toString();
        countryCodesCache.insert(ip, countryCode);
        countryNamesCache.insert(countryCode, countryName);
        qCDebug(jtIpToLocation) << "Data received IP:" << ip << " Lang:" << language << " country code:" << countryCode << " country name:" << countryName;

        emit ipResolved(ip);
    }
    else{
        qCDebug(jtIpToLocation) << "error requesting " << ip << ". Returning an empty location!";
    }

    reply->deleteLater();
}

// At moment the current api is geoip.nekudo.com. Another option is https://freegeoip.net/json/
void WebIpToLocationResolver::requestDataFromWebService(const QString &ip){
    qCDebug(jtIpToLocation) << "requesting ip " << ip ;

    QNetworkRequest request;
    QString serviceUrl = "http://geoip.nekudo.com/api/";

    // http://geoip.nekudo.com/api/{ip}/{language}/{type}
    request.setUrl(QUrl(serviceUrl + ip + "/" + currentLanguage + "/short"));

    QNetworkReply* reply = httpClient.get(request);
    reply->setProperty("ip", QVariant(ip));
    reply->setProperty("language", QVariant(currentLanguage));
    QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(replyError(QNetworkReply::NetworkError)));
}

void WebIpToLocationResolver::replyError(QNetworkReply::NetworkError e){
    qCCritical(jtIpToLocation) << "Reply error! " << e;
}

QString WebIpToLocationResolver::sanitizeLanguageCode(const QString &languageCode)
{
    if (languageCode.size() > 2)
        return languageCode.toLower().left(2);

    return languageCode;
}

Geo::Location WebIpToLocationResolver::resolve(const QString &ip, const QString &languageCode){

    //check for language changes
    QString code = sanitizeLanguageCode(languageCode);
    if (code != currentLanguage) {
        saveCountryNamesToFile(); //save cached country names before change the language
        currentLanguage = code;
        loadCountryNamesFromFile(currentLanguage); //update the country names QMap
    }

    if (countryCodesCache.contains(ip)) {
        QString countryCode = countryCodesCache[ip];
        if (countryNamesCache.contains(countryCode)) {
            QString countryName = countryNamesCache[countryCode];
            qCDebug(jtIpToLocation) << "cache hit for " << ip;
            return Location(countryName, countryCode);
        }
    }

    if (!ip.isEmpty()) {
        requestDataFromWebService(ip);
    }
    return Location();//empty location, the next request for same ip probabily return from cache
}

QString WebIpToLocationResolver::buildFileNameFromLanguage(const QString &languageCode)
{
   return COUNTRY_NAMES_FILE_PREFIX + "_" + languageCode + ".bin";
}

void WebIpToLocationResolver::loadCountryNamesFromFile(const QString &languageCode)
{
    QString fileName = buildFileNameFromLanguage(languageCode);
    if (populateQMapFromFile(fileName, countryNamesCache)){
        qCDebug(jtIpToLocation) << countryNamesCache.size() << " cached country names loaded, translated to " << languageCode;
    }
    else
    {
        qCritical() << "Can't open the file " << fileName;
    }
    qDebug() << countryNamesCache.size() << " items in countryNamesCache Map after load from " << languageCode;
}

void WebIpToLocationResolver::loadCountryCodesFromFile()
{
    if (populateQMapFromFile(COUNTRY_CODES_FILE, countryCodesCache))
        qCDebug(jtIpToLocation) << countryCodesCache.size() << " cached country codes loaded from file!";
    else
        qCritical() << "Can't open the file " << COUNTRY_CODES_FILE;
}

bool WebIpToLocationResolver::populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map)
{
    map.clear();
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if(cacheFile.open(QFile::ReadOnly)){
        QDataStream fileStream(&cacheFile);
        fileStream >> map;
        return true;
    }
    return false;
}
