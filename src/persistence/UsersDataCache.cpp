#include "UsersDataCache.h"
#include "log/logging.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>

using namespace Persistence;

//+++++++++++++++++++++++++++++++++++++++
CacheEntry::CacheEntry(QString userIp, QString userName, int channelID, bool muted, float gain, float pan, float boost)
    :userIp(userIp), userName(userName), channelID(channelID), muted(muted), gain(gain), pan(pan), boost(boost){

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

CacheEntry UsersDataCache::getUserCacheEntry(QString userIp, QString userName, int channelID){
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

QString UsersDataCache::getUserUniqueKey(QString userIp, QString userName, int channelID){
    //the unique key is just a string...
    return userIp + userName + QString::number(channelID);
}


void UsersDataCache::loadCacheEntriesFromFile(){
    //load tracks cache content from file
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    QFile cacheFile(cacheDir.absoluteFilePath(CACHE_FILE_NAME));
    if(cacheFile.open(QFile::ReadOnly)){
        QTextStream inputStream(&cacheFile);
        while(!inputStream.atEnd()){
            QString line = inputStream.readLine();
            if(!line.isNull() && !line.isEmpty()){
                QStringList parts = line.split(";");
                if(!parts.isEmpty()){
                    /*  Fields order in cache file:
                            [0]User IP
                            [1]User Name
                            [2]Channel ID
                            [3]Gain
                            [4]Pan
                            [5]Mute
                            [6]Boost     */
                    QString userIP = parts.at(0);
                    QString userName = (parts.size() >= 1) ? parts.at(1) : "";
                    int channelID = (parts.size() >= 2) ? parts.at(2).toInt() : 0;
                    float gain = (parts.size() >= 3) ? parts.at(3).toFloat() : 1.0;//unity gain as default
                    float pan = (parts.size() >= 4) ? parts.at(4).toFloat() : 0.0;//center as default
                    bool mute = (parts.size() >= 5) ? (bool)(parts.at(5).toInt()) : false;//not muted as default
                    float boost = (parts.size() >= 6) ? parts.at(6).toFloat() : 1.0;//unity gain/boost as default
                    QString userUniqueKey = getUserUniqueKey(userIP, userName, channelID);
                    cacheEntries.insert(userUniqueKey, CacheEntry(userIP, userName, channelID, mute, gain, pan, boost));
                }
            }
        }
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
        QTextStream stream(&cacheFile);
        foreach (const QString& userUniqueKey, cacheEntries.keys()) {
            CacheEntry entry = cacheEntries[userUniqueKey];
            stream << entry.getUserIP() << ";"
                   << entry.getUserName() << ";"
                   << entry.getChannelID() << ";"
                   << entry.getGain() << ";"
                   << entry.getPan() << ";"
                   << (int)(entry.isMuted()) << ";"
                   << entry.getBoost()
                   << endl;
        }
        qCDebug(jtCache) << cacheEntries.size() << " items stored in tracks cache file!";
    }
    else{
       qCCritical(jtCache) << "Can't open the tracks cache file in" << QFileInfo(cacheFile).absoluteFilePath();
    }

}

//++++++++++++++++++


