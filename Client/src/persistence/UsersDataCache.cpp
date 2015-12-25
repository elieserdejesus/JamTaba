#include "UsersDataCache.h"
#include "log/logging.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QDataStream>

using namespace Persistence;

const bool CacheEntry::DEFAULT_MUTED = false;
const float CacheEntry::DEFAULT_GAIN = 1.0f;
const float CacheEntry::DEFAULT_PAN = 0.0f;
const float CacheEntry::DEFAULT_BOOST = 1.0f;
const float CacheEntry::PAN_MAX = 4.0f;
const float CacheEntry::PAN_MIN = -4.0f;

// well formed address is an acceptable.
// no need to validate the number within 8 bits.
QRegExp CacheEntry::ipPattern("(?:\\d{1,3}\\.){3}(\\d{1,3}|x)");

QRegExp CacheEntry::namePattern("[a-zA-Z0-9_]{1,64}");


QDataStream & operator<<(QDataStream &stream, const CacheEntry &entry)
{
    return stream
        << entry.getUserIP()
        << entry.getUserName()
        << entry.getChannelID()
        << entry.isMuted()
        << entry.getGain()
        << entry.getPan()
        << entry.getBoost();
}

QDataStream & operator>>(QDataStream &stream, CacheEntry &entry)
{
    QString userIp, userName;
    quint8 channelID;
    bool muted;
    float gain, pan, boost;

    stream >> userIp >> userName >> channelID >> muted >> gain >> pan >> boost;

    entry.setUserIP(userIp);
    entry.setUserName(userName);
    entry.setChannelID(channelID);
    entry.setMuted(muted);
    entry.setGain(gain);
    entry.setPan(pan);
    entry.setBoost(boost);

    return stream;
}


//+++++++++++++++++++++++++++++++++++++++

CacheEntry::CacheEntry(const QString& userIp, const QString& userName, quint8 channelID)
{
    setUserIP(userIp);
    setUserName(userName);
    setChannelID(channelID);
    setMuted(DEFAULT_MUTED);
    setGain(DEFAULT_GAIN);
    setPan(DEFAULT_PAN);
    setBoost(DEFAULT_BOOST);
}

void CacheEntry::setUserIP(const QString& userIp)
{
    if (ipPattern.exactMatch(userIp)) {
        this->userIp = userIp;
    }
    else {
        qCDebug(jtCache) << "invalid ip address: " << userIp;
    }
}

void CacheEntry::setUserName(const QString& userName)
{
    if (namePattern.exactMatch(userName)) {
        this->userName = userName;
    }
    else {
        qCDebug(jtCache) << "invalid name: " << userName;
    }
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

    if (pan != this->pan) {
        qCDebug(jtCache) << "pan " << pan << " was out of range";
    }
}

void CacheEntry::setBoost(float boost)
{
    this->boost = boost;
}

void CacheEntry::setGain(float gain)
{
    this->gain = gain;
}

UsersDataCache::UsersDataCache()
    :CACHE_FILE_NAME("tracks_cache.bin")
{
    loadCacheEntriesFromFile();
}

UsersDataCache::~UsersDataCache(){
    if(!cacheEntries.isEmpty()){
        writeCacheEntriesToFile();
    }
}

CacheEntry UsersDataCache::getUserCacheEntry(const QString& userIp, const QString& userName, quint8 channelID){
    QString userUniqueKey = getUserUniqueKey(userIp, userName, channelID);
    if(cacheEntries.contains(userUniqueKey)){
        return cacheEntries[userUniqueKey];
    }
    return CacheEntry(userIp, userName, channelID);//return a entry using default values for pan, gain, mute, etc.
}

void UsersDataCache::updateUserCacheEntry(CacheEntry entry){
    QString userKey = getUserUniqueKey(entry.getUserIP(), entry.getUserName(), entry.getChannelID());
    cacheEntries.insert(userKey, entry);//replace the last value or insert
}

QString UsersDataCache::getUserUniqueKey(const QString& userIp, const QString& userName, quint8 channelID){
    //the unique key is just a string...
    return userIp + userName + QString::number(channelID);
}


void UsersDataCache::loadCacheEntriesFromFile(){
    //load tracks cache content from file
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if(cacheFile.open(QFile::ReadOnly)){
        QDataStream stream(&cacheFile);
        stream.skipRawData(12); // skip header
        stream >> cacheEntries;
    }
    qCDebug(jtCache) << "Tracks cache items loaded from file: " << cacheEntries.size();
}

void UsersDataCache::writeCacheEntriesToFile(){
    qCDebug(jtCache) << "Saving cache file";
    //save cache content into file
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if(cacheFile.open(QFile::WriteOnly)){
        qCDebug(jtCache) << "Tracks cache file opened to write.";
        QDataStream stream(&cacheFile);

        // TODO: move thise to header. this is a qucik dirty implementation
        quint32 signature = 0x4a544232; // "JTB2"
        quint32 revision = 1;
        quint32 offset = 12; // sizeof(signature) + sizeof(revision) + sizeof(offset)
        stream << signature << revision << offset;

        stream << cacheEntries;

        qCDebug(jtCache) << cacheEntries.size() << " items stored in tracks cache file!";
    }
    else{
       qCCritical(jtCache) << "Can't open the tracks cache file in" << QFileInfo(cacheFile).absoluteFilePath();
    }
}

//++++++++++++++++++


