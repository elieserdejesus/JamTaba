#include "UsersDataCache.h"
#include "log/Logging.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDataStream>
#include "Configurator.h"
#include "CacheHeader.h"

using namespace Persistence;

const quint32 UsersDataCacheHeader::REVISION = 2; // added low cut in revision 2

const bool CacheEntry::DEFAULT_MUTED = false;
const bool CacheEntry::DEFAULT_LOW_CUT = false;
const float CacheEntry::DEFAULT_GAIN = 1.0f;
const float CacheEntry::DEFAULT_PAN = 0.0f;
const float CacheEntry::DEFAULT_BOOST = 1.0f;
const float CacheEntry::PAN_MAX = 4.0f;
const float CacheEntry::PAN_MIN = -4.0f;

// well formed address is an acceptable.
// no need to validate the number within 8 bits.
QRegExp CacheEntry::ipPattern("(?:\\d{1,3}\\.){3}(\\d{1,3}|x)");


QDataStream &operator<<(QDataStream &stream, const CacheEntry &entry)
{
    return stream
           << entry.getUserIP()
           << entry.getUserName()
           << entry.getChannelID()
           << entry.isMuted()
           << entry.getGain()
           << entry.getPan()
           << entry.getBoost()
           << entry.isLowCutActivated();
}

QDataStream &operator>>(QDataStream &stream, CacheEntry &entry)
{
    QString userIp, userName;
    quint8 channelID;
    bool muted, lowCutActivated;
    float gain, pan, boost;

    stream >> userIp >> userName >> channelID >> muted >> gain >> pan >> boost >> lowCutActivated;

    entry.setUserIP(userIp);
    entry.setUserName(userName);
    entry.setChannelID(channelID);
    entry.setMuted(muted);
    entry.setGain(gain);
    entry.setPan(pan);
    entry.setBoost(boost);
    entry.setLowCutActivated(lowCutActivated);

    return stream;
}

// +++++++++++++++++++++++++++++++++++++++
CacheEntry::CacheEntry(const QString &userIp, const QString &userName, quint8 channelID)
{
    setUserIP(userIp);
    setUserName(userName);
    setChannelID(channelID);
    setMuted(DEFAULT_MUTED);
    setGain(DEFAULT_GAIN);
    setPan(DEFAULT_PAN);
    setBoost(DEFAULT_BOOST);
    setLowCutActivated(DEFAULT_LOW_CUT);
}

void CacheEntry::setLowCutActivated(bool activated)
{
    this->lowCutActivated = activated;
}

void CacheEntry::setUserIP(const QString &userIp)
{
    if (ipPattern.exactMatch(userIp))
        this->userIp = userIp;
    else
        qCDebug(jtCache) << "invalid ip address: " << userIp;
}

void CacheEntry::setUserName(const QString &userName)
{
    this->userName = userName;
}

void CacheEntry::setChannelID(quint8 channelID)
{
    this->channelID = channelID;
}

void CacheEntry::setMuted(bool muted)
{
    this->muted = muted;
}

void CacheEntry::setPan(float pan)
{
    this->pan = qBound(PAN_MIN, pan, PAN_MAX);

    if (pan != this->pan)
        qCDebug(jtCache) << "pan " << pan << " was out of range";
}

void CacheEntry::setBoost(float boost)
{
    this->boost = boost;
}

void CacheEntry::setGain(float gain)
{
    this->gain = gain;
}

UsersDataCache::UsersDataCache(const QDir &cacheDir)
    :CACHE_FILE_NAME("tracks_cache.bin"),
     cacheDir(cacheDir)
{
    //check if the tracks_cache_bin file is in the old dir and copy the file to the 'cache' dir.
    //This piece of code will be deleted in future versions.
    QDir baseDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    QFile oldCacheFile(baseDir.absoluteFilePath(CACHE_FILE_NAME));
    if (oldCacheFile.exists()) {
        if (oldCacheFile.rename(cacheDir.absoluteFilePath(CACHE_FILE_NAME)))
            qDebug() << CACHE_FILE_NAME << " copyed to the new cache folder!";
        else
            qDebug() << "Error when copying " << CACHE_FILE_NAME << " to the new cache folder!";
    }

    loadCacheEntriesFromFile();
}

UsersDataCache::~UsersDataCache()
{
    if (!cacheEntries.isEmpty())
        writeCacheEntriesToFile();
}

CacheEntry UsersDataCache::getUserCacheEntry(const QString &userIp, const QString &userName,
                                             quint8 channelID)
{
    QString userUniqueKey = getUserUniqueKey(userIp, userName, channelID);
    if (cacheEntries.contains(userUniqueKey))
        return cacheEntries[userUniqueKey];
    return CacheEntry(userIp, userName, channelID);// return a entry using default values for pan, gain, mute, etc.
}

void UsersDataCache::updateUserCacheEntry(CacheEntry entry)
{
    QString userKey
        = getUserUniqueKey(entry.getUserIP(), entry.getUserName(), entry.getChannelID());
    cacheEntries.insert(userKey, entry);// replace the last value or insert
}

QString UsersDataCache::getUserUniqueKey(const QString &userIp, const QString &userName,
                                         quint8 channelID)
{
    // the unique key is just a string...
    return userIp + userName + QString::number(channelID);
}

void UsersDataCache::loadCacheEntriesFromFile()
{
    // load tracks cache content from file
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if (cacheFile.open(QFile::ReadOnly)) {
        QDataStream stream(&cacheFile);

        CacheHeader cacheHeader;
        stream >> cacheHeader;
        quint32 expectedHeaderRevision = UsersDataCacheHeader::REVISION;
        if (cacheHeader.isValid(expectedHeaderRevision))
            stream >> cacheEntries;
        else
            qCritical() << "Invalid cache header when loading users data cache.";

        qCDebug(jtCache) << "Tracks cache items loaded from file: " << cacheEntries.size();
    }
}

void UsersDataCache::writeCacheEntriesToFile()
{
    qCDebug(jtCache) << "Saving cache file";

    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if (cacheFile.open(QFile::WriteOnly)) {
        qCDebug(jtCache) << "Tracks cache file opened to write.";
        QDataStream stream(&cacheFile);

        CacheHeader cacheHeader(UsersDataCacheHeader::REVISION);
        stream << cacheHeader;

        stream << cacheEntries;

        qCDebug(jtCache) << cacheEntries.size() << " items stored in tracks cache file!";
    } else {
        qCCritical(jtCache) << "Can't open the tracks cache file in"
                            << QFileInfo(cacheFile).absoluteFilePath();
    }
}

// ++++++++++++++++++
