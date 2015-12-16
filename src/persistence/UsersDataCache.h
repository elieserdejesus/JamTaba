#ifndef USERSDATACACHE_H
#define USERSDATACACHE_H

#include <QString>
#include <QMap>

/***
  This class is used to store/remember the users level, pan, mute and boost. When a user enter in the jam
  the data is recovered/remembered from this cache.
 */

namespace Persistence {

class CacheEntry{//cache entries are per channel, not per user.
public:
    CacheEntry(QString userIp, QString userName, int channelID, bool muted=false, float gain=1.0, float pan=0.0, float boost=1.0);
    CacheEntry(){}//default constructor just to use in QMap without pointers

    inline bool isMuted() const{ return muted; }
    inline float getGain() const{ return gain; }
    inline float getPan() const{ return pan; }
    inline float getBoost() const{ return boost; }
    inline QString getUserIP() const{ return userIp; }
    inline QString getUserName() const{ return userName; }
    inline int getChannelID() const{ return channelID; }

    inline void setMuted(bool muted){ this->muted = muted; }
    inline void setPan(float pan){ this->pan = pan; }
    inline void setBoost(float boost) { this->boost = boost; }
    inline void setGain(float gain){ this->gain = gain; }
private:
    QString userIp;
    QString userName;
    int channelID;
    bool muted;
    float gain;//fader level
    float pan;
    float boost;
};


//++++++++++++++++++++++++++++++++
class UsersDataCache
{
public:
    UsersDataCache();
    ~UsersDataCache();
    CacheEntry getUserCacheEntry(QString userIp, QString userName, int channelID);//return default values for pan, gain and mute if user is not cached yet
    void updateUserCacheEntry(CacheEntry entry);
private:
    QMap<QString, CacheEntry> cacheEntries;

    static QString getUserUniqueKey(QString userIp, QString userName, int channelID);

    void loadCacheEntriesFromFile();
    void writeCacheEntriesToFile();

    const QString CACHE_FILE_NAME;

};

}//namespace

#endif // USERSDATACACHE_H
