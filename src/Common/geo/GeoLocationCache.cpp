#include "GeoLocationCache.h"

#include <QtDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDataStream>

#include "log/Logging.h"
#include "persistence/CacheHeader.h"
#include "Location.h"

using namespace geo;

const QString GeoLocationCache::COUNTRY_CODES_FILE = "country_codes_cache.bin";
const QString GeoLocationCache::COUNTRY_NAMES_FILE_PREFIX = "country_names_cache"; // the language code will be concatenated
const QString GeoLocationCache::LAT_LONG_CACHE_FILE = "lat_long_cache.bin";

const quint32 GeoLocationCache::COUNTRY_NAMES_CACHE_REVISION = 1;
const quint32 GeoLocationCache::COUNTRY_CODES_CACHE_REVISION = 1;
const quint32 GeoLocationCache::LAT_LONG_CACHE_REVISION = 1;

GeoLocationCache::GeoLocationCache(const QDir &cacheDir) :
    cacheDir(cacheDir),
    currentLanguage("en") // using english as default language
{
    loadCountryCodesFromFile();
    loadLatLongsFromFile();

    if (!needLoadTheOldCache()) {
        loadCountryNamesFromFile("en"); // loading the english country names by default
    } else {
        loadOldCacheContent();
        deleteOldCacheFile();
    }
}

GeoLocationCache::~GeoLocationCache()
{
    saveCountryCodesToFile();
    saveCountryNamesToFile();
    saveLatLongsToFile();
}

bool GeoLocationCache::isCached(const QString &ip) const
{
    return !(getLocation(ip).isUnknown());
}

Location GeoLocationCache::getLocation(const QString &ip) const
{
    if (countryCodesCache.contains(ip)) {
        auto countryCode = countryCodesCache[ip];
        if (countryNamesCache.contains(countryCode)) {
            auto countryName = countryNamesCache[countryCode];
            if (latLongCache.contains(ip)) {
                auto latitude = latLongCache[ip].x();
                auto longitude = latLongCache[ip].y();
                return Location(countryName, countryCode, latitude, longitude);
            }
        }
    }

    return Location(); // return UNKNOWN location
}

void GeoLocationCache::setCurrentLanguage(const QString &languageCode)
{
    saveCountryNamesToFile(); // save cached country names before change the language

    currentLanguage = languageCode;

    loadCountryNamesFromFile(languageCode); // update the country names QMap
}

void GeoLocationCache::add(const QString &ip, const Location &location)
{
    auto countryCode = location.getCountryCode();

    countryCodesCache.insert(ip, countryCode);
    countryNamesCache.insert(countryCode, location.getCountryName());
    latLongCache.insert(ip, location.getLatLong());
}

void GeoLocationCache::loadLatLongsFromFile()
{
    QString fileName = LAT_LONG_CACHE_FILE;
    quint32 expectedCacheHeaderRevision = LAT_LONG_CACHE_REVISION;
    if (populateQMapFromFile(fileName, latLongCache, expectedCacheHeaderRevision))
        qCDebug(jtIpToLocation) << latLongCache.size() << " lat,long pairs loaded in cache";
    else
        qCritical() << "Can't open the file " << fileName;
}

void GeoLocationCache::loadCountryNamesFromFile(const QString &languageCode)
{
    QString fileName = buildFileNameFromLanguage(languageCode);
    quint32 expectedCacheHeaderRevision = COUNTRY_NAMES_CACHE_REVISION;
    if (populateQMapFromFile(fileName, countryNamesCache, expectedCacheHeaderRevision))
        qCDebug(jtIpToLocation) << countryNamesCache.size() << " cached country names loaded, translated to " << languageCode;
    else
        qCritical() << "Can't open the file " << fileName;
}

void GeoLocationCache::loadCountryCodesFromFile()
{
    quint32 expectedCacheHeaderRevision = COUNTRY_CODES_CACHE_REVISION;
    if (populateQMapFromFile(COUNTRY_CODES_FILE, countryCodesCache, expectedCacheHeaderRevision))
        qCDebug(jtIpToLocation) << countryCodesCache.size()
                                << " cached country codes loaded from file!";
    else
        qCritical() << "Can't open the file " << COUNTRY_CODES_FILE;
}

bool GeoLocationCache::populateQMapFromFile(const QString &fileName, QMap<QString, QString> &map, quint32 expectedCacheHeaderRevision)
{
    map.clear();
    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if (cacheFile.open(QFile::ReadOnly)) {
        QDataStream stream(&cacheFile);
        CacheHeader cacheHeader;
        stream >> cacheHeader;
        if (cacheHeader.isValid(expectedCacheHeaderRevision)) {
            stream >> map;
            return true;
        } else {
            qCritical() << "Cache header is not valid in " << fileName;
        }
    }
    return false;
}

bool GeoLocationCache::populateQMapFromFile(const QString &fileName, QMap<QString, QPointF> &map, quint32 expectedCacheHeaderRevision)
{
    map.clear();
    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if (cacheFile.open(QFile::ReadOnly)) {
        QDataStream stream(&cacheFile);
        CacheHeader cacheHeader;
        stream >> cacheHeader;
        if (cacheHeader.isValid(expectedCacheHeaderRevision)) {
            stream >> map;
            return true;
        } else {
            qCritical() << "Cache header is not valid in " << fileName;
        }
    }
    return false;
}

void GeoLocationCache::saveLatLongsToFile()
{
    if (latLongCache.isEmpty())
        return;

    QString filename = LAT_LONG_CACHE_FILE;
    quint32 cacheRevision = LAT_LONG_CACHE_REVISION;
    if (saveMapToFile(filename, latLongCache, cacheRevision))
        qCDebug(jtIpToLocation) << latLongCache.size() << " lat,lng pairs stored in " << filename;
    else
        qCritical() << "Can't save lat,lng pairs in the file " << filename;
}

void GeoLocationCache::saveCountryNamesToFile()
{
    QString filename = buildFileNameFromLanguage(currentLanguage);
    quint32 cacheRevision = COUNTRY_NAMES_CACHE_REVISION;
    if (saveMapToFile(filename, countryNamesCache, cacheRevision))
        qCDebug(jtIpToLocation) << countryNamesCache.size() << " country names stored in "
                                << filename;
    else
        qCritical() << "Can't save country names in the file " << filename;
}

void GeoLocationCache::saveCountryCodesToFile()
{
    quint32 cacheRevision = COUNTRY_CODES_CACHE_REVISION;
    if (saveMapToFile(COUNTRY_CODES_FILE, countryCodesCache, cacheRevision))
        qCDebug(jtIpToLocation) << countryCodesCache.size() << " country codes stored in "
                                << COUNTRY_CODES_FILE;
    else
        qCritical() << "Can't save country codes in the file " << COUNTRY_CODES_FILE;
}

bool GeoLocationCache::saveMapToFile(const QString &fileName, const QMap<QString, QPointF> &map, quint32 cacheHeaderRevision)
{
    if (map.isEmpty())
        return true;

    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if (cacheFile.open(QFile::WriteOnly)) {
        QDataStream stream(&cacheFile);
        CacheHeader cacheHeader(cacheHeaderRevision);
        stream << cacheHeader;
        stream << map;
        return true;
    }
    return false;
}

bool GeoLocationCache::saveMapToFile(const QString &fileName, const QMap<QString, QString> &map, quint32 cacheHeaderRevision)
{
    if (map.isEmpty())
        return true;

    QFile cacheFile(cacheDir.absoluteFilePath(fileName));
    if (cacheFile.open(QFile::WriteOnly)) {
        QDataStream stream(&cacheFile);
        CacheHeader cacheHeader(cacheHeaderRevision);
        stream << cacheHeader;
        stream << map;
        return true;
    }
    return false;
}

QString GeoLocationCache::buildFileNameFromLanguage(const QString &languageCode)
{
    return COUNTRY_NAMES_FILE_PREFIX + "_" + languageCode + ".bin";
}

void GeoLocationCache::loadOldCacheContent()
{
    // load cache content from the old 'cache.bin' file. This code will be delete in future versions
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath("cache.bin"));
    if (cacheFile.open(QFile::ReadOnly)) {
        QTextStream inputStream(&cacheFile);
        while (!inputStream.atEnd())
        {
            QString line = inputStream.readLine();
            if (!line.isNull() && !line.isEmpty()) {
                QStringList parts = line.split(";");
                if (!parts.isEmpty()) {
                    QString ip = parts.at(0);
                    QString countryName = (parts.size() >= 1) ? parts.at(1) : "";
                    QString countryCode = (parts.size() >= 2) ? parts.at(2) : "";
                    countryCodesCache.insert(ip, countryCode);
                    countryNamesCache.insert(countryCode, countryName);
                }
            }
        }
    }
}

void GeoLocationCache::deleteOldCacheFile()
{
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath("cache.bin"));
    cacheFile.remove();
}

bool GeoLocationCache::needLoadTheOldCache()
{
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    return QFile(cacheDir.absoluteFilePath("cache.bin")).exists();
}
