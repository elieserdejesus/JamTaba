#ifndef FREEGEOIPTOLOCATIONRESOLVER_H
#define FREEGEOIPTOLOCATIONRESOLVER_H

#include "IpToLocationResolver.h"
#include "GeoLocationCache.h"

#include <QMap>
#include <QLoggingCategory>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QDir>

namespace geo
{
    class WebIpToLocationResolver : public IpToLocationResolver
    {
        Q_OBJECT

    public:
        explicit WebIpToLocationResolver(const QDir &cacheDir);
        ~WebIpToLocationResolver() override;
        geo::Location resolve(const QString &ip, const QString &languageCode) override;

    private:

        QNetworkAccessManager httpClient;

        void requestDataFromWebService(const QString &ip, int retryCount);

        static QString sanitizeLanguageCode(const QString &languageCode);

        static bool canTranslateCountryName(const QString &currentLanguage);

        GeoLocationCache cache;

    private slots:
        void replyFinished(QNetworkReply *);
        void replyError(QNetworkReply::NetworkError);
    };
} // namespace

#endif // FREEGEOIPTOLOCATIONRESOLVER_H
