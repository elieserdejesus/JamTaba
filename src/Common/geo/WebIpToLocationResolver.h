#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include <QMap>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

namespace Geo {
class WebIpToLocationResolver : public IpToLocationResolver
{
    Q_OBJECT
public:
    WebIpToLocationResolver();
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
    bool populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map);

    //saving
    void saveCountryCodesToFile();
    void saveCountryNamesToFile();
    bool saveMapToFile(const QString &fileName, const QMap<QString, QString> &map);

    static QString buildFileNameFromLanguage(const QString &languageCode);
    static QString sanitizeLanguageCode(const QString &languageCode);

    QString currentLanguage;

    static const QString COUNTRY_CODES_FILE;
    static const QString COUNTRY_NAMES_FILE_PREFIX;

private slots:
    void replyFinished(QNetworkReply *);
    void replyError(QNetworkReply::NetworkError);
};
}

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
