#ifndef USERSDATACACHE_H
#define USERSDATACACHE_H

#include <QString>
#include <QMap>
#include <QRegExp>
#include <QDir>

/**

  This class is used to store/remember the users level, pan, mute and boost. When a user enter in the jam
  the data is recovered/remembered from this cache.

 */

namespace persistence {

struct UsersDataCacheHeader {
    static const quint32 REVISION;
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

    inline int getLowCutState() const
    {
        return lowCutState;
    }

    inline int getInstrumentIndex() const
    {
        return instrumentIndex;
    }

    void setUserIP(const QString &userIp);
    void setUserName(const QString &userName);
    void setChannelID(quint8 channelID);
    void setMuted(bool muted);
    void setPan(float pan);
    void setBoost(float boost);
    void setGain(float gain);
    void setLowCutState(quint8 state);
    void setInstrumentIndex(qint8 index);
    bool hasValidInstrumentIndex() const;

    static QRegExp ipPattern;

    static const bool DEFAULT_MUTED;
    static const quint8 DEFAULT_LOW_CUT_STATE;
    static const float DEFAULT_GAIN;
    static const float DEFAULT_PAN;
    static const float DEFAULT_BOOST;
    static const float PAN_MAX;
    static const float PAN_MIN;
    static const qint8 DEFAULT_INSTRUMENT_INDEX;
private:
    QString userIp;
    QString userName;
    quint8 channelID;
    bool muted;
    float gain; // fader level
    float pan;
    float boost;
    quint8 lowCutState;
    qint8 instrumentIndex;
};

// ++++++++++++++++++++++++++++++++

class UsersDataCache
{
public:
    explicit UsersDataCache(const QDir &cacheDir);
    ~UsersDataCache();

    // return default values for pan, gain and mute if user is not cached yet
    CacheEntry getUserCacheEntry(const QString &userIp, const QString &userName, quint8 channelID);

    void updateUserCacheEntry(CacheEntry entry);
private:
    QMap<QString, CacheEntry> cacheEntries;

    QDir cacheDir;

    static QString getUserUniqueKey(const QString &userIp, const QString &userName,
                                    quint8 channelID);

    void loadCacheEntriesFromFile();
    void writeCacheEntriesToFile();

    const QString CACHE_FILE_NAME;
};

}// namespace

#endif // USERSDATACACHE_H
