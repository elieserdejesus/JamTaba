#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <QMap>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>

namespace geo {

class WebIpToLocationResolver : public IpToLocationResolver
{
    Q_OBJECT

public:
    explicit WebIpToLocationResolver(const QDir &cacheDir);
    ~WebIpToLocationResolver() override;
    geo::Location resolve(const QString &ip, const QString &languageCode) override;

private:
    QMap<QString, QString> countryCodesCache; // IP -> country code (2 upper case letters)
    QMap<QString, QString> countryNamesCache; // country code => translated country name
    QMap<QString, QPointF> latLongCache;      // IP => QPointF(latitude, longitude)
    QNetworkAccessManager httpClient;

    void requestDataFromWebService(const QString &ip, int retryCount);

    // loading
    void loadCountryCodesFromFile();
    void loadCountryNamesFromFile(const QString &languageCode);
    void loadLatLongsFromFile();
    bool populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map, quint32 expectedCacheHeaderRevision);
    bool populateQMapFromFile(const QString &fileName, QMap<QString, QPointF> &map, quint32 expectedCacheHeaderRevision);

    bool needLoadTheOldCache();
    void loadOldCacheContent(); // these functions are used to handle the old 'cache.bin' content used until the version 2.0.13. This will be deleted in future.
    void deleteOldCacheFile();

    //saving
    void saveCountryCodesToFile();
    void saveCountryNamesToFile();
    void saveLatLongsToFile();
    bool saveMapToFile(const QString &fileName, const QMap<QString, QString> &map, quint32 cacheHeaderRevision);
    bool saveMapToFile(const QString &fileName, const QMap<QString, QPointF> &map, quint32 cacheHeaderRevision);

    static QString buildFileNameFromLanguage(const QString &languageCode);
    static QString sanitizeLanguageCode(const QString &languageCode);

    static bool canTranslateCountryName(const QString &currentLanguage);

    QString currentLanguage;

    QDir cacheDir;

    static const QString COUNTRY_CODES_FILE;
    static const QString COUNTRY_NAMES_FILE_PREFIX;
    static const QString LAT_LONG_CACHE_FILE;

    static const quint32 COUNTRY_CODES_CACHE_REVISION;
    static const quint32 COUNTRY_NAMES_CACHE_REVISION;
    static const quint32 LAT_LONG_CACHE_REVISION;

private slots:
    void replyFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);

};

} // namespace

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
