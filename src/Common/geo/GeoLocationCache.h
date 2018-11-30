#ifndef GEO_LOCATION_CACHE
#define GEO_LOCATION_CACHE

#include <QMap>
#include <QString>
#include <QPointF>
#include <QDir>

namespace geo
{
    class Location;

    class GeoLocationCache
    {
    public:
        GeoLocationCache(const QDir &cacheDir);
        ~GeoLocationCache();

        bool isCached(const QString &ip) const;
        Location getLocation(const QString &ip) const;

        void setCurrentLanguage(const QString &languageCode);
        QString getCurrentLanguage() const;

    public slots:
        void add(const QString &ip, const Location &location);

    private:
        QMap<QString, QString> countryCodesCache; // IP -> country code (2 upper case letters)
        QMap<QString, QString> countryNamesCache; // country code => translated country name
        QMap<QString, QPointF> latLongCache;    // IP => QPointF(latitude, longitude)

        QDir cacheDir;
        QString currentLanguage;

        static const QString COUNTRY_CODES_FILE;
        static const QString COUNTRY_NAMES_FILE_PREFIX;
        static const QString LAT_LONG_CACHE_FILE;

        static const quint32 COUNTRY_CODES_CACHE_REVISION;
        static const quint32 COUNTRY_NAMES_CACHE_REVISION;
        static const quint32 LAT_LONG_CACHE_REVISION;

        // loading
        void loadCountryCodesFromFile();
        void loadCountryNamesFromFile(const QString &languageCode);
        void loadLatLongsFromFile();
        bool populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map, quint32 expectedCacheHeaderRevision);
        bool populateQMapFromFile(const QString &fileName, QMap<QString, QPointF> &map, quint32 expectedCacheHeaderRevision);

        bool needLoadTheOldCache();
        void loadOldCacheContent(); // these functions are used to handle the old 'cache.bin' content used until the version 2.0.13. This will be deleted in future.
        void deleteOldCacheFile();

        // saving
        void saveCountryCodesToFile();
        void saveCountryNamesToFile();
        void saveLatLongsToFile();
        bool saveMapToFile(const QString &fileName, const QMap<QString, QString> &map, quint32 cacheHeaderRevision);
        bool saveMapToFile(const QString &fileName, const QMap<QString, QPointF> &map, quint32 cacheHeaderRevision);

        static QString buildFileNameFromLanguage(const QString &languageCode);
    };

    inline QString GeoLocationCache::getCurrentLanguage() const
    {
        return currentLanguage;
    }
}

#endif
