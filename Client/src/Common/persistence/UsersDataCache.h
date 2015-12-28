#ifndef USERSDATACACHE_H
#define USERSDATACACHE_H

#include <QString>
#include <QMap>
#include <QRegExp>

/***
  This class is used to store/remember the users level, pan, mute and boost. When a user enter in the jam
  the data is recovered/remembered from this cache.
 */

namespace Persistence {
/**
 * @brief The CacheEntryHeader struct
 */
struct UsersDataCacheHeader {
    static const quint32 SIGNATURE;
    static const quint32 REVISION;
    static const quint32 SIZE;
};

class CacheEntry // cache entries are per channel, not per user.
{
public:
    CacheEntry(const QString &userIp, const QString &userName, quint8 channelID);
    CacheEntry()
    {
    }

    inline bool isMuted() const
    {
        return muted;
    }

    inline float getGain() const
    {
        return gain;
    }

    inline float getPan() const
    {
        return pan;
    }

    inline float getBoost() const
    {
        return boost;
    }

    inline QString getUserIP() const
    {
        return userIp;
    }

    inline QString getUserName() const
    {
        return userName;
    }

    inline quint8 getChannelID() const
    {
        return channelID;
    }

    void setUserIP(const QString &userIp);
    void setUserName(const QString &userName);
    void setChannelID(quint8 channelID);
    void setMuted(bool muted);
    void setPan(float pan);
    void setBoost(float boost);
    void setGain(float gain);

    static QRegExp ipPattern;

    static const bool DEFAULT_MUTED;
    static const float DEFAULT_GAIN;
    static const float DEFAULT_PAN;
    static const float DEFAULT_BOOST;
    static const float PAN_MAX;
    static const float PAN_MIN;
private:
    QString userIp;
    QString userName;
    quint8 channelID;
    bool muted;
    float gain;// fader level
    float pan;
    float boost;
};

// ++++++++++++++++++++++++++++++++
class UsersDataCache
{
public:
    UsersDataCache();
    ~UsersDataCache();

    // return default values for pan, gain and mute if user is not cached yet
    CacheEntry getUserCacheEntry(const QString &userIp, const QString &userName, quint8 channelID);

    void updateUserCacheEntry(CacheEntry entry);
private:
    QMap<QString, CacheEntry> cacheEntries;

    static QString getUserUniqueKey(const QString &userIp, const QString &userName,
                                    quint8 channelID);

    void loadCacheEntriesFromFile();
    void writeCacheEntriesToFile();

    const QString CACHE_FILE_NAME;
};
}// namespace

#endif // USERSDATACACHE_H
