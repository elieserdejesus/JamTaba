#pragma once

#include <QList>
#include <QMap>

class NinjamServer;

class NinjamPublicServersParser {

public:
    virtual QList<NinjaMServer*> getPublicServers() = 0;
    virtual void shutdown(){}
    
};
//++++++++++++++++++++++
/**
 * esta classe usa os dois parsers, porque o parser do ninbot nao
 * retorna o maxusers, e preciso disso para determinar quando uma sala do ninjam
 * ja esta cheia
 */
class MixedPublicServersParser : public NinjamPublicServersParser {

public:
    virtual QList<NinjamServer*> getPublicServers(){
        QList<NinjamServer*> servers;
        try {
            if (service != null && service.isShutdown() || service == null) {
                service = Executors.newCachedThreadPool();
            }
            Future<Collection<NinjaMServer>> autoSongFuture = service.submit(new ParserTask(new AutoSongNinjamDotComServersParser(true)));
            Future<Collection<NinjaMServer>> ninbotFuture = service.submit(new ParserTask(new NinbotDotComServersParser()));
            Collection<NinjaMServer> autoSongServers = autoSongFuture.get();
            Collection<NinjaMServer> ninbotServers = ninbotFuture.get();
            Map<String, NinjaMServer> autoSongServersMap = buildServersMap(autoSongServers);
            for (NinjaMServer ninbotServer : ninbotServers) {
                NinjaMServer autoSongServer = autoSongServersMap.get(ninbotServer.getUniqueName());
                if (autoSongServer != null) {
                    ninbotServer.setMaxUsers(autoSongServer.getMaxUsers());
                }
            }
            return ninbotServers;
        } catch (Exception e) {
            LOGGER.log(Level.SEVERE, e.getMessage(), e);
        }
        return servers;
    }


    virtual void shutdown() {
        if (service != null && !service.isShutdown()) {
            service.shutdownNow();
        }
    }

    QMap<String, NinjamServer> buildServersMap(QList<NinjamServer*> servers) {
        Map<String, NinjaMServer> map = new HashMap<String, NinjaMServer>();
        for (NinjaMServer s : servers) {
            map.put(s.getUniqueName(), s);
        }
        return map;
    }

};
