#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <QMap>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>

namespace Geo {

class WebIpToLocationResolver : public IpToLocationResolver
{
    Q_OBJECT
public:
    WebIpToLocationResolver(const QDir &cacheDir);
    ~WebIpToLocationResolver();
    Geo::Location resolve(const QString &ip, const QString &languageCode) override;
private:
    QMap<QString, QString> countryCodesCache; // IP -> country code (2 upper case letters)
    QMap<QString, QString> countryNamesCache;// country code => translated country name
    QNetworkAccessManager httpClient;

    void requestDataFromWebService(const QString &ip);

    //loading
    void loadCountryCodesFromFile();
    void loadCountryNamesFromFile(const QString &languageCode);
    bool populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map, quint32 expectedCacheHeaderRevision);

    bool needLoadTheOldCache();
    void loadOldCacheContent(); //these functions are used to handle the old 'cache.bin' content used until the version 2.0.13. This will be deleted in future.
    void deleteOldCacheFile();

    //saving
    void saveCountryCodesToFile();
    void saveCountryNamesToFile();
    bool saveMapToFile(const QString &fileName, const QMap<QString, QString> &map, quint32 cacheHeaderRevision);

    static QString buildFileNameFromLanguage(const QString &languageCode);
    static QString sanitizeLanguageCode(const QString &languageCode);

    QString currentLanguage;

    QDir cacheDir;

    static const QString COUNTRY_CODES_FILE;
    static const QString COUNTRY_NAMES_FILE_PREFIX;

    static const quint32 COUNTRY_CODES_CACHE_REVISION;
    static const quint32 COUNTRY_NAMES_CACHE_REVISION;

private slots:
    void replyFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);
};
}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
